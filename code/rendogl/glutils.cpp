#include "render.h"



void MipMap(GLenum format, INT sw, INT sh, const BYTE *src, BYTE *dest );



//
// My scale image
//
void ScaleImage(GLenum format, INT inwidth, INT inheight, const BYTE *in,
				 INT outwidth, INT outheight, BYTE *out ) {

	INT			i, j;
	const BYTE	*inrow;
	DWORD		frac, fracstep;

	fracstep = inwidth*0x10000/outwidth;

	switch(format){
		case GL_RGB:
			for(i=0; i<outheight; i++, out+=outwidth*3) {
				inrow = in + 3*inwidth*(i*inheight/outheight);
				frac = fracstep >> 1;
				for(j=0; j<outwidth*3; j+=3) {
					out[j]   = inrow[(frac>>16)*3];
					out[j+1] = inrow[(frac>>16)*3+1];
					out[j+2] = inrow[(frac>>16)*3+2];
					frac += fracstep;
				}
			}
			break;
		case GL_RGBA:
			for(i=0; i<outheight; i++, out+=outwidth*4) {
				inrow = in + 4*inwidth*(i*inheight/outheight);
				frac = fracstep >> 1;
				for (j=0 ; j<outwidth*4 ; j+=4) {
					out[j]   = inrow[(frac>>16)*4  ];
					out[j+1] = inrow[(frac>>16)*4+1];
					out[j+2] = inrow[(frac>>16)*4+2];
					out[j+3] = inrow[(frac>>16)*4+3];
					frac += fracstep;
				}
			}
			break;
		case GL_LUMINANCE:
		case GL_ALPHA:
			for(i=0; i<outheight; i++, out+=outwidth*4) {
				inrow = in + 1*inwidth*(i*inheight/outheight);
				frac = fracstep >> 1;
				for (j=0; j<outwidth*1; j+=1) {
					out[j]   = inrow[(frac>>16)*1  ];
					frac += fracstep;
				}
			}
		break;
		case GL_LUMINANCE_ALPHA:
			for(i=0; i<outheight; i++, out+=outwidth*4) {
				inrow = in + 2*inwidth*(i*inheight/outheight);
				frac = fracstep >> 1;
				for(j=0; j<outwidth*1; j+=2) {
					out[j]   = inrow[(frac>>16)*2  ];
					out[j+1] = inrow[(frac>>16)*2+1];
					frac += fracstep;
				}
			}
		break;
	}
}



// FIXME: there are some errors!
void MipMap(GLenum format, INT sw, INT sh, const BYTE *src, BYTE *dest ) {
	INT bpp;
	INT dw, dh;
	bool filterw = true;
	bool filterh = true;

	if(sw<=2){
		dw=1;
		filterw=false;
	}
	else dw=sw/2;
	
	if(sh<=2){
		dh=1;
		filterh=false;
	}
	else dh=sh/2;

		
	switch (format) {
		case GL_ALPHA:
		case GL_LUMINANCE:
			bpp = 1;
		break;
		case GL_LUMINANCE_ALPHA:
			bpp = 2;
		break;
		case GL_RGB:
			bpp = 3;
		break;
		case GL_RGBA:
			bpp = 4;
		break;
		default:
			return;
	}

	INT row, col, b;

	for (row = 0; row < dh; row++) {
		for (col = 0; col < dw; col++) {
			for(b=0; b<bpp; b++){
				int s0, s1, s2, s3;
				s0 = *(src+b);
		
				if (filterw) s1 = *(src+b+bpp);
				else s1 = s0;

				if (filterh) {
					s2 = *(src+b+sw*bpp);
					if (filterw) s3 = *(src+b+sw*bpp+bpp);
					else s3 = s2;
				}
				else {
					s2 = s0;
					s3 = s1;
				}

				*dest++ = (s0+s1+s2+s3+2) >> 2;
			}
			src += 2*bpp;
		}
		src += sw*bpp;
	}
}



void BuildMipmaps(GLenum components, INT width, INT height, GLenum format, const BYTE *data){
	INT w, h, bpp, neww, newh;
	INT level;
	const BYTE *image=data;
	BYTE *newimage;

	if (width < 1 || height < 1) return;

	w=width;
	h=height;

	switch (format) {
		case GL_ALPHA:
		case GL_LUMINANCE:
			bpp = 1;
		break;
		case GL_LUMINANCE_ALPHA:
			bpp = 2;
		break;
		case GL_RGB:
			bpp = 3;
		break;
		case GL_RGBA:
			bpp = 4;
		break;
		default:
			return;
			//bpp = 0;
	}


	level = 0;
	while( true ){
		glTexImage2D( GL_TEXTURE_2D, level, components, w, h, 0, format, GL_UNSIGNED_BYTE, image );

		if(w==1 && h==1)  break;

		neww = (w<2) ? 1 : w/2;
		newh = (h<2) ? 1 : h/2;
		newimage = (BYTE *) malloc( (neww+4) * newh * bpp );
		if (!newimage) return;

		if(newh<4 || neww<4)
			glScaleImage( format, w, h, GL_UNSIGNED_BYTE, image, neww, newh, GL_UNSIGNED_BYTE, newimage );
		else
			MipMap( format, w, h, image, newimage );

		if (image!=data) {
			free((void *) image);
		}

		image = newimage;

		w = neww;
		h = newh;
		level++;
	}

	if (image!=data) {
		free( (void *) image );
	}

	return;
}







//
// Given the source data and dimensions, computes an anti-aliased MIP-map
// of dimensions (sw >> 1, sh >> 1) into the destination array.
// Uses a simple 2x2 box filter.
//
void HalfScaleFilterBox(int sw, int sh, DWORD* src, DWORD* dest){
	INT dw = sw >> 1;
	INT dh = sh >> 1;

	bool filterw = true;
	bool filterh = true;
	if (dw == 0) {
		dw = 1;
		filterw = false;
	}
	if (dh == 0) {
		dh = 1;
		filterh = false;
	}

	DWORD* p = src;
	DWORD* q = dest;
	INT row, col;
	for (row = 0; row < dh; row++) {
		for (col = 0; col < dw; col++) {
			// 2x2 box filter.
			DWORD s0, s1, s2, s3;
			s0 = *p;
			if (filterw) s1 = *(p+1);
			else s1 = s0;
			if (filterh) {
				s2 = *(p+sw);
				if (filterw) s3 = *(p+sw+1);
				else s3 = s2;
			} else {
				s2 = s0;
				s3 = s1;
			}
			DWORD r = ((s0 & 0x0FF) + (s1 & 0x0FF) + (s2 & 0x0FF) + (s3 & 0x0FF) + 2) >> 2;
			DWORD g = (((s0 >> 8) & 0x0FF) + ((s1 >> 8) & 0x0FF) + ((s2 >> 8) & 0x0FF) + ((s3 >> 8) & 0x0FF) + 2) >> 2;
			DWORD b = (((s0 >> 16) & 0x0FF) + ((s1 >> 16) & 0x0FF) + ((s2 >> 16) & 0x0FF) + ((s3 >> 16) & 0x0FF) + 2) >> 2;
			DWORD a = ((s0 >> 24) + (s1 >> 24) + (s2 >> 24) + (s3 >> 24) + 2) >> 2;

			*q++ = (r) | (g << 8) | (b << 16) | (a << 24);
			p += 2;
		}
		p += sw;
	}
}

/*
//
// Given the source data and dimensions, computes an anti-aliased
// MIP-map of dimensions (sw >> 1, sh >> 1) into the destination array.
// Uses a truncated sinc-function filter, if possible; otherwise a
// simple box filter.
//
void HalfScaleFilterSinc(int sw, int sh, DWORD* src, DWORD* dest){
	int dw = sw >> 1;
	int dh = sh >> 1;

	if (dw == 0 || dh == 0) {
		// One or the other dimension doesn't scale.  Use simpler 2x2 box filter.
		HalfScaleFilterBox(sw, sh, src, dest);
		return;
	}

	// Use a 4x4 convolution filter, based on sinc function.

	// Truncated sin(x)/x * sin(y)/y, sampled at -3pi/2, -pi/2, pi/2, 3pi/2
	float Kernel[4][4] = {    // The scaling is arbitrary.
		{ 0.045032, -0.135095, -0.135095,  0.045032 },
		{ -0.135095, 0.405285,  0.405285, -0.135095 },
		{ -0.135095, 0.405285,  0.405285, -0.135095 },
		{ 0.045032, -0.135095, -0.135095,  0.045032 },
	};

	DWORD* sp = src;
	int row, col;
	for (row = 0; row < dh; row++, sp += sw * 2) {
		for (col = 0; col < dw; col++) {

			float r = 0, g = 0, b = 0, a = 0, weight = 0;

			int sourcerow = row * 2 - 1;

			int i, j;
			for (j = 0; j < 4; j++, sourcerow++) {
				if (sourcerow < 0) continue;
				if (sourcerow >= sh) break;

				int sourcecol = col * 2 - 1;

				for (i = 0; i < 4; i++, sourcecol++) {
					if (sourcecol < 0) continue;
					if (sourcecol >= sw) break;

					DWORD s = src[sourcerow * sw + sourcecol];
					float w = Kernel[j][i];

					weight += w;
					r += ((s) & 0x0FF) * w;
					g += ((s >> 8) & 0x0FF) * w;
					b += ((s >> 16) & 0x0FF) * w;
					a += ((s >> 24) & 0x0FF) * w;
				}
			}

			float rw = 1.0 / weight;
			r *= rw;
			g *= rw;
			b *= rw;
			a *= rw;
			dest[row * dw + col] = iclamp(0, r, 255) + (iclamp(0, g, 255) << 8) +
									(iclamp(0, b, 255) << 16) + (iclamp(0, a, 255) << 24);
		}
	}
}
*/










GLint __gluBuild2DMipMaps(GLint component, GLsizei w, GLsizei h, GLenum format, GLenum type, const void *data){
	GLint		level;


	glTexImage2D( GL_TEXTURE_2D, 0, component, w, h, 0, format, type, data );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );


	// Setup the transformation pipeline for 2D.
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity();
	//glOrtho( 0, 640, 480, 0, 0.0, 1.0 );

	glViewport( 0, 0, 640, 480);
	glScissor(0, 0, 640, 480);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0, 640, 0, 480, 0.0f, 1.0f );

 

	// create a new mipmap level until both of the dimensions equal 1. 
	for( level = 1; (w != 1) && (h != 1 ); level++ ){
		
		// Shift the dimensions to the next level.
		w = w >> 1;
		h = h >> 1;

		// As mipmap filtering is off... we can use the level 0 texture to
		// draw this level for us so that the cards filtering gets used.
		glBegin( GL_QUADS );
		glTexCoord2f( 0.0, 0.0 );
		glVertex2i( 0, 0 );

		glTexCoord2f( 0.0, 1.0 );
		glVertex2i( 0, h );

		glTexCoord2f( 1.0, 1.0 );
		glVertex2i( w, h );

		glTexCoord2f( 1.0, 0.0 );
		glVertex2i( w, 0 );
		glEnd();

		glTexImage2D( GL_TEXTURE_2D, level, component, w, h, 0, format, GL_UNSIGNED_BYTE, data);

		/* Read the pixels back into this texture level. */
		glCopyTexImage2D( GL_TEXTURE_2D, level, component, 0, 0, w, h, 0 );
	}


	return 0;
}
















/*
 * Compute ceiling of integer quotient of A divided by B:
 */
#define CEILING( A, B )  ( (A) % (B) == 0 ? (A)/(B) : (A)/(B)+1 )



#ifdef EPSILON
#undef EPSILON
#endif
#define EPSILON 0.001


/* To work around optimizer bug in MSVC4.1 */
#if defined(__WIN32__) && !defined(OPENSTEP)
void dummy(GLuint j, GLuint k){
}
#else
#define dummy(J, K)
#endif


GLint glScaleImage( GLenum format,
                              GLint widthin, GLint heightin,
                              GLenum typein, const void *datain,
                              GLint widthout, GLint heightout,
                              GLenum typeout, void *dataout )
{
   GLint components, i, j, k;
   GLfloat *tempin, *tempout;
   GLfloat sx, sy;
   GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
   GLint packrowlength, packalignment, packskiprows, packskippixels;
   GLint sizein, sizeout;
   GLint rowstride, rowlen;


   /* Determine number of components per pixel */
   switch (format) {
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         components = 1;
	 break;
      case GL_LUMINANCE_ALPHA:
	 components = 2;
	 break;
      case GL_RGB:
	 components = 3;
	 break;
      case GL_RGBA:
#ifdef GL_EXT_abgr
      case GL_ABGR_EXT:
#endif
	 components = 4;
	 break;
      default:
	 return GL_INVALID_ENUM;
   }

   /* Determine bytes per input datum */
   switch (typein) {
      case GL_UNSIGNED_BYTE:	sizein = sizeof(GLubyte);	break;
      case GL_BYTE:		sizein = sizeof(GLbyte);	break;
      case GL_UNSIGNED_SHORT:	sizein = sizeof(GLushort);	break;
      case GL_SHORT:		sizein = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	sizein = sizeof(GLuint);	break;
      case GL_INT:		sizein = sizeof(GLint);		break;
      case GL_FLOAT:		sizein = sizeof(GLfloat);	break;
      case GL_BITMAP:
	 /* not implemented yet */
      default:
	 return GL_INVALID_ENUM;
   }

   /* Determine bytes per output datum */
   switch (typeout) {
      case GL_UNSIGNED_BYTE:	sizeout = sizeof(GLubyte);	break;
      case GL_BYTE:		sizeout = sizeof(GLbyte);	break;
      case GL_UNSIGNED_SHORT:	sizeout = sizeof(GLushort);	break;
      case GL_SHORT:		sizeout = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	sizeout = sizeof(GLuint);	break;
      case GL_INT:		sizeout = sizeof(GLint);	break;
      case GL_FLOAT:		sizeout = sizeof(GLfloat);	break;
      case GL_BITMAP:
	 /* not implemented yet */
      default:
	 return GL_INVALID_ENUM;
   }

   /* Get glPixelStore state */
   glGetIntegerv( GL_UNPACK_ROW_LENGTH, &unpackrowlength );
   glGetIntegerv( GL_UNPACK_ALIGNMENT, &unpackalignment );
   glGetIntegerv( GL_UNPACK_SKIP_ROWS, &unpackskiprows );
   glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &unpackskippixels );
   glGetIntegerv( GL_PACK_ROW_LENGTH, &packrowlength );
   glGetIntegerv( GL_PACK_ALIGNMENT, &packalignment );
   glGetIntegerv( GL_PACK_SKIP_ROWS, &packskiprows );
   glGetIntegerv( GL_PACK_SKIP_PIXELS, &packskippixels );

   /* Allocate storage for intermediate images */
   tempin = (GLfloat *) malloc( widthin * heightin
			        * components * sizeof(GLfloat) );
   if (!tempin) {
      return GL_OUT_OF_MEMORY;
   }
   tempout = (GLfloat *) malloc( widthout * heightout
		 	         * components * sizeof(GLfloat) );
   if (!tempout) {
      free( tempin );
      return GL_OUT_OF_MEMORY;
   }


   /*
    * Unpack the pixel data and convert to floating point
    */

   if (unpackrowlength>0) {
      rowlen = unpackrowlength;
   }
   else {
      rowlen = widthin;
   }
   if (sizein >= unpackalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = unpackalignment/sizein
	        * CEILING( components * rowlen * sizein, unpackalignment );
   }

   switch (typein) {
      case GL_UNSIGNED_BYTE:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLubyte *ubptr = (GLubyte *) datain
	                   + i * rowstride
			   + unpackskiprows * rowstride
			   + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *ubptr++;
	    }
	 }
	 break;
      case GL_BYTE:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLbyte *bptr = (GLbyte *) datain
	                 + i * rowstride
			 + unpackskiprows * rowstride
			 + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *bptr++;
	    }
	 }
	 break;
      case GL_UNSIGNED_SHORT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLushort *usptr = (GLushort *) datain
	                    + i * rowstride
			    + unpackskiprows * rowstride
			    + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *usptr++;
	    }
	 }
	 break;
      case GL_SHORT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLshort *sptr = (GLshort *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *sptr++;
	    }
	 }
	 break;
      case GL_UNSIGNED_INT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLuint *uiptr = (GLuint *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *uiptr++;
	    }
	 }
	 break;
      case GL_INT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLint *iptr = (GLint *) datain
	                + i * rowstride
			+ unpackskiprows * rowstride
			+ unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *iptr++;
	    }
	 }
	 break;
      case GL_FLOAT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLfloat *fptr = (GLfloat *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = *fptr++;
	    }
	 }
	 break;
      default:
	 return GL_INVALID_ENUM;
   }


   /*
    * Scale the image!
    */

   if (widthout > 1)
      sx = (GLfloat) (widthin-1) / (GLfloat) (widthout-1);
   else
      sx = (GLfloat) (widthin-1);
   if (heightout > 1)
      sy = (GLfloat) (heightin-1) / (GLfloat) (heightout-1);
   else
      sy = (GLfloat) (heightin-1);

/*#define POINT_SAMPLE*/
#ifdef POINT_SAMPLE
   for (i=0;i<heightout;i++) {
      GLint ii = i * sy;
      for (j=0;j<widthout;j++) {
	 GLint jj = j * sx;

	 GLfloat *src = tempin + (ii * widthin + jj) * components;
	 GLfloat *dst = tempout + (i * widthout + j) * components;

	 for (k=0;k<components;k++) {
	    *dst++ = *src++;
	 }
      }
   }
#else
   if (sx<1.0 && sy<1.0) {
      /* magnify both width and height:  use weighted sample of 4 pixels */
      GLint i0, i1, j0, j1;
      GLfloat alpha, beta;
      GLfloat *src00, *src01, *src10, *src11;
      GLfloat s1, s2;
      GLfloat *dst;

      for (i=0;i<heightout;i++) {
	 i0 = i * sy;
	 i1 = i0 + 1;
	 if (i1 >= heightin) i1 = heightin-1;
/*	 i1 = (i+1) * sy - EPSILON;*/
	 alpha = i*sy - i0;
	 for (j=0;j<widthout;j++) {
	    j0 = j * sx;
	    j1 = j0 + 1;
	    if (j1 >= widthin) j1 = widthin-1;
/*	    j1 = (j+1) * sx - EPSILON; */
	    beta = j*sx - j0;

	    /* compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
	    src00 = tempin + (i0 * widthin + j0) * components;
	    src01 = tempin + (i0 * widthin + j1) * components;
	    src10 = tempin + (i1 * widthin + j0) * components;
	    src11 = tempin + (i1 * widthin + j1) * components;

	    dst = tempout + (i * widthout + j) * components;

	    for (k=0;k<components;k++) {
	       s1 = *src00++ * (1.0-beta) + *src01++ * beta;
	       s2 = *src10++ * (1.0-beta) + *src11++ * beta;
	       *dst++ = s1 * (1.0-alpha) + s2 * alpha;
	    }
	 }
      }
   }
   else {
      /* shrink width and/or height:  use an unweighted box filter */
      GLint i0, i1;
      GLint j0, j1;
      GLint ii, jj;
      GLfloat sum, *dst;

      for (i=0;i<heightout;i++) {
	 i0 = i * sy;
	 i1 = i0 + 1;
	 if (i1 >= heightin) i1 = heightin-1; 
/*	 i1 = (i+1) * sy - EPSILON; */
	 for (j=0;j<widthout;j++) {
	    j0 = j * sx;
	    j1 = j0 + 1;
	    if (j1 >= widthin) j1 = widthin-1;
/*	    j1 = (j+1) * sx - EPSILON; */

	    dst = tempout + (i * widthout + j) * components;

	    /* compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
	    for (k=0;k<components;k++) {
	       sum = 0.0;
	       for (ii=i0;ii<=i1;ii++) {
		  for (jj=j0;jj<=j1;jj++) {
		     sum += *(tempin + (ii * widthin + jj) * components + k);
		  }
	       }
	       sum /= (j1-j0+1) * (i1-i0+1);
	       *dst++ = sum;
	    }
	 }
      }
   }
#endif


   /*
    * Return output image
    */

   if (packrowlength>0) {
      rowlen = packrowlength;
   }
   else {
      rowlen = widthout;
   }
   if (sizeout >= packalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = packalignment/sizeout
	        * CEILING( components * rowlen * sizeout, packalignment );
   }

   switch (typeout) {
      case GL_UNSIGNED_BYTE:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLubyte *ubptr = (GLubyte *) dataout
	                   + i * rowstride
			   + packskiprows * rowstride
			   + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *ubptr++ = (GLubyte) tempout[k++];
	    }
	 }
	 break;
      case GL_BYTE:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLbyte *bptr = (GLbyte *) dataout
	                 + i * rowstride
			 + packskiprows * rowstride
			 + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *bptr++ = (GLbyte) tempout[k++];
	    }
	 }
	 break;
      case GL_UNSIGNED_SHORT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLushort *usptr = (GLushort *) dataout
	                    + i * rowstride
			    + packskiprows * rowstride
			    + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *usptr++ = (GLushort) tempout[k++];
	    }
	 }
	 break;
      case GL_SHORT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLshort *sptr = (GLshort *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *sptr++ = (GLshort) tempout[k++];
	    }
	 }
	 break;
      case GL_UNSIGNED_INT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLuint *uiptr = (GLuint *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *uiptr++ = (GLuint) tempout[k++];
	    }
	 }
	 break;
      case GL_INT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLint *iptr = (GLint *) dataout
	                + i * rowstride
			+ packskiprows * rowstride
			+ packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *iptr++ = (GLint) tempout[k++];
	    }
	 }
	 break;
      case GL_FLOAT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLfloat *fptr = (GLfloat *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *fptr++ = tempout[k++];
	    }
	 }
	 break;
      default:
	 return GL_INVALID_ENUM;
   }


   /* free temporary image storage */
   free( tempin );
   free( tempout );

   return 0;
}




/*
 * Find the value nearest to n which is also a power of two.
 */
static GLint round2( GLint n )
{
   GLint m;

   for (m=1; m<n; m*=2)
     ;

   /* m>=n */
   if (m-n <= n-m/2) {
      return m;
   }
   else {
      return m/2;
   }
}


/*
 * Given an pixel format and datatype, return the number of bytes to
 * store one pixel.
 */
static GLint bytes_per_pixel( GLenum format, GLenum type )
{
   GLint n, m;

   switch (format) {
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
	 n = 1;
	 break;
      case GL_LUMINANCE_ALPHA:
	 n = 2;
	 break;
      case GL_RGB:
	 n = 3;
	 break;
      case GL_RGBA:
#ifdef GL_EXT_abgr
      case GL_ABGR_EXT:
#endif
	 n = 4;
	 break;
      default:
	 n = 0;
   }

   switch (type) {
      case GL_UNSIGNED_BYTE:	m = sizeof(GLubyte);	break;
      case GL_BYTE:		m = sizeof(GLbyte);	break;
      case GL_BITMAP:		m = 1;			break;
      case GL_UNSIGNED_SHORT:	m = sizeof(GLushort);	break;
      case GL_SHORT:		m = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	m = sizeof(GLuint);	break;
      case GL_INT:		m = sizeof(GLint);	break;
      case GL_FLOAT:		m = sizeof(GLfloat);	break;
      default:			m = 0;
   }

   return n * m;
}





GLint glBuild2DMipmaps( GLenum target, GLint components,
                                  GLint width, GLint height, GLenum format,
                                  GLenum type, const void *data )
{
   GLint w, h, maxsize;
   void *image, *newimage;
   GLint neww, newh, level, bpp;
   int error;

   if (width < 1 || height < 1)
      return GL_INVALID_VALUE;

   glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxsize );

   w = round2( width );
   if (w>maxsize) {
      w = maxsize;
   }
   h = round2( height );
   if (h>maxsize) {
      h = maxsize;
   }

   bpp = bytes_per_pixel( format, type );
   if (bpp==0) {
      /* probably a bad format or type enum */
      return GL_INVALID_ENUM;
   }

   if (w!=width || h!=height) {
      /* must rescale image to get "top" mipmap texture image */
      image = malloc( (w+4) * h * bpp );
      if (!image) {
	 return GL_OUT_OF_MEMORY;
      }
      error = glScaleImage( format, width, height, type, data,
			     w, h, type, image );
      if (error) {
	 return error;
      }
   }
   else {
      image = (void *) data;
   }

   level = 0;
   while (1) {
      glTexImage2D( target, level, components, w, h, 0, format, type, image );

      if (w==1 && h==1)  break;

      neww = (w<2) ? 1 : w/2;
      newh = (h<2) ? 1 : h/2;
      newimage = malloc( (neww+4) * newh * bpp );
      if (!newimage) {
	 return GL_OUT_OF_MEMORY;
      }

      error =  glScaleImage( format, w, h, type, image, neww, newh, type, newimage );

      if (error) {
	 return error;
      }

      if (image!=data) {
	 free( image );
      }
      image = newimage;

      w = neww;
      h = newh;
      level++;
   }

   if (image!=data) {
      free( image );
   }

   return 0;
}

