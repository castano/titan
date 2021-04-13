/*===========================================================================
	Title: tex.cpp
	Author: Ignacio Castaño Aguado
	Description: texture manager

===========================================================================*/
#include "rfrontend.h"

#ifdef WIN32
#include "ijl.h"		// under Windows use intel jpeg library
#else
extern "C" {
#include "jpeglib.h"	// eoc use jpeglib
}
#endif



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IOutputDevice *out;
extern IRender *render;
extern Error *err;
extern vars * cvars;





/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

#pragma pack (push, 1)
typedef struct {	// TGA file header
	byte id_length, colormap_type, image_type;
	word colormap_index, colormap_length;
	byte colormap_size;
	word x_origin, y_origin, width, height;
	byte pixel_size, attributes;
} TgaHeader;

// .WAL texture file format
#define MIPLEVELS			4
typedef struct {
	char		name[32];
	unsigned	width, height;
	unsigned	offsets[MIPLEVELS];	// four mipmaps stored
	char		animname[32];		// next frame in animation chain
	int			flags;
	int			contents;
	int			value;
}miptex_t;
#pragma pack (pop, 1)

typedef enum {
	IT_TGA,
	IT_JPG,
	IT_WAL,
}image_type;





/*---------------------------------------------------------------------------
	Constants:
---------------------------------------------------------------------------*/

// hardcoded palette:
const static byte pal[768]={
	0, 0, 0, 15, 15, 15, 31, 31, 31, 47, 47, 47, 63, 63, 63, 75, 75, 75, 
	91, 91, 91, 107, 107, 107, 123, 123, 123, 139, 139, 139, 155, 155, 155, 
	171, 171, 171, 187,187, 187, 203, 203, 203, 219, 219, 219, 235, 235, 235,
	99, 75, 35, 91, 67, 31, 83, 63, 31, 79, 59, 27, 71, 55, 27, 63, 47, 23,
	59, 43, 23, 51, 39, 19, 47, 35, 19, 43, 31, 19, 39, 27, 15, 35, 23, 15,
	27, 19, 11, 23, 15, 11, 19, 15, 7, 15, 11, 7, 95, 95, 111, 91, 91, 103,
	91, 83, 95, 87, 79, 91, 83, 75, 83, 79, 71, 75, 71, 63, 67, 63, 59, 59,
	59, 55, 55, 51, 47, 47, 47, 43, 43, 39, 39, 39, 35, 35, 35, 27, 27, 27,
	23, 23, 23, 19, 19, 19, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	235, 159, 39, 203, 139, 35, 175, 119, 31, 147, 99, 27, 119, 79, 23,
	91, 59, 15, 63, 39, 11, 35, 23, 7, 167, 59, 43, 159, 47, 35, 151, 43, 27,
	139, 39, 19, 127, 31, 15, 115, 23, 11, 103, 23, 7, 87, 19, 0, 75, 15, 0,
	67, 15, 0, 59, 15, 0, 51, 11, 0, 43, 11, 0, 35, 11, 0, 27, 7, 0,
	19, 7, 0, 123, 95, 75, 115, 87, 67, 107, 83, 63, 103, 79, 59, 95, 71, 55,
	87, 67, 51, 83, 63, 47, 75, 55, 43, 67, 51, 39, 63, 47, 35, 55, 39, 27,
	47, 35, 23, 39, 27, 19, 31, 23, 15, 23, 15, 11, 15, 11, 7, 111, 59, 23,
	95, 55, 23, 83, 47, 23, 67, 43, 23, 55, 35, 19, 39, 27, 15, 27, 19, 11,
	15, 11, 7, 179, 91, 79, 191, 123, 111, 203, 155, 147, 215, 187, 183,
	203, 215, 223, 179, 199, 211, 159, 183, 195, 135, 167, 183, 115, 151, 167,
	91, 135, 155, 71, 119, 139, 47, 103, 127, 23, 83, 111, 19, 75, 103,
	15, 67, 91, 11, 63, 83, 7, 55, 75, 7, 47, 63, 7, 39, 51, 0, 31, 43,
	0, 23, 31, 0, 15, 19, 0, 7, 11, 0, 0, 0, 139, 87, 87, 131, 79, 79,
	123, 71, 71, 115, 67, 67, 107, 59, 59, 99, 51, 51, 91, 47, 47, 87, 43, 43,
	75, 35, 35, 63, 31, 31, 51, 27, 27, 43, 19, 19, 31, 15, 15, 19, 11, 11,
	11, 7, 7, 0, 0, 0, 151, 159, 123, 143, 151, 115, 135, 139, 107, 127, 131, 99,
	119, 123, 95, 115, 115, 87, 107, 107, 79, 99, 99, 71, 91, 91, 67, 79, 79, 59,
	67, 67, 51, 55, 55, 43, 47, 47, 35, 35, 35, 27, 23, 23, 19, 15, 15, 11,
	159, 75, 63, 147, 67, 55, 139, 59, 47, 127, 55, 39, 119, 47, 35, 107, 43, 27,
	99, 35, 23, 87, 31, 19, 79, 27, 15, 67, 23, 11, 55, 19, 11, 43, 15, 7,
	31, 11, 7, 23, 7, 0, 11, 0, 0, 0, 0, 0, 119, 123, 207, 111, 115, 195, 
	103, 107, 183, 99, 99, 167, 91, 91, 155, 83, 87, 143, 75, 79, 127, 71, 71, 115,
	63, 63, 103, 55, 55, 87, 47, 47, 75, 39, 39, 63, 35, 31, 47, 27, 23, 35,
	19, 15, 23, 11, 7, 7, 155, 171, 123, 143, 159, 111, 135, 151, 99, 123, 139, 87,
	115, 131, 75, 103, 119, 67, 95, 111, 59, 87, 103, 51, 75, 91, 39, 63, 79, 27,
	55, 67, 19, 47, 59, 11, 35, 47, 7, 27, 35, 0, 19, 23, 0, 11, 15, 0, 0, 255, 0,
	35, 231, 15, 63, 211, 27, 83, 187, 39, 95, 167, 47, 95, 143, 51, 95, 123, 51,
	255, 255, 255, 255, 255, 211, 255, 255, 167, 255, 255, 127, 255, 255, 83,
	255, 255, 39, 255, 235, 31, 255, 215, 23, 255, 191, 15, 255, 171, 7,
	255, 147, 0, 239, 127, 0, 227, 107, 0, 211, 87, 0, 199, 71, 0, 183, 59, 0,
	171, 43, 0, 155, 31, 0, 143, 23, 0, 127, 15, 0, 115, 7, 0, 95, 0, 0,
	71, 0, 0, 47, 0, 0, 27, 0, 0, 255, 255, 255, 255, 255, 255, 255, 0, 0,
	0, 0, 255, 43, 43, 35, 27, 27, 23, 19, 19, 15, 235, 151, 127, 195, 115, 83,
	159, 87, 51, 123, 63, 27, 235, 211, 199, 199, 171, 155, 167, 139, 119,
	135, 107, 87, 159, 91, 83
};




/*---------------------------------------------------------------------------
	Public Methods:
---------------------------------------------------------------------------*/

//
// Constructor allocates space for the references
//
TexMng::TexMng(int num){
	guard(TexMng::TexMng);
	max_refs = num;
	num_refs=0;
	texrefs = new TexRefs[num];
	appMemzero(texrefs, sizeof(TexRefs)*num);
	unguard;
}


//
// Deallocate space of the texrefs
//
TexMng::~TexMng(void){
	guard(TexMng::~TexMng);
	delete [] texrefs;
	unguard;
}


//
// Increases a texinfo count, if already loaded, or reserves it if not loaded
//
texinfo *TexMng::AddTexinfo(const char *name, int flags){
	guard(TexMng::AddTexinfo);
	char *temp_name;
	int i;


	temp_name = new char[strlen(name)+1];			// TODO: use namehunk
	strcpy(temp_name, name);
	f_StripExtension(temp_name);

	
	// search for name
	for(i=0; i<max_refs; i++){

		if(!texrefs[i].name) continue;		// free slot

		if(!strcmp(texrefs[i].name, temp_name)){
			// allready referenced, increase reference counter
			// FIXME: what happens if different flags?
			texrefs[i].count++;
			return texrefs[i].info;
		}
	}


	if(num_refs>=max_refs){
		// TODO: do something better here !!!
		err->CriticalError(UNKNOWN_ERROR, "Out of texinfo refs!");
		return NULL;
	}


	// find a free reference and allocate it
	for(i=0; i<max_refs; i++){
		if(texrefs[i].count==0 && texrefs[i].info==NULL){
			texrefs[i].name	= new char[strlen(temp_name)+1+4]; // +4 to add later the extension
			strcpy(texrefs[i].name, temp_name);
			texrefs[i].count = 1;
			texrefs[i].flags = flags;
			texrefs[i].info = new texinfo;
			texrefs[i].info->num=(~0);

			num_refs++;
			return texrefs[i].info;
		}
	}

	return NULL;		// shoudn't happen!
	unguard;
}


//
// decreases count of the given texinfo
//
void TexMng::DeleteTexinfo(texinfo *info){
	guard(TexMng::DeleteTexinfo);

	if(info==NULL) return;		// FIXME: write check macros!

	for(int i=0; i<max_refs; i++){
		if(texrefs[i].info==info){
			if(texrefs[i].count) texrefs[i].count--;
			return;
		}
	}

	unguard;
}


//
// reset all the counters
//
void TexMng::ResetAll(void){
	guard(TexMng::ResetAll);
	for(int i=0; i<max_refs; i++){
		texrefs[i].count=0;
	}
	unguard;
}



//
// loads unloaded shaders, deletes unreferenced shaders
//
void TexMng::Update(void){
	guard(ShaderMng::Update);
	int i;


	for(i=0; i<max_refs; i++){
		if( texrefs[i].count ) { 
			if( texrefs[i].info->num==(~0) )
				LoadTexture(&texrefs[i]);
		}
		else {
			if(texrefs[i].info){
				render->UnregisterTexInfo(texrefs[i].info);
				delete texrefs[i].info; texrefs[i].info=NULL;
				delete texrefs[i].name; texrefs[i].name=NULL;
				num_refs--;
			}
		}
	}
	out->Insert("TexMng: %d textures in memory\n", num_refs);

	unguard;
}





//
// given a texrefs, loads the texture, sends it to the driver and delete it.
//
void TexMng::LoadTexture(TexRefs *tr){
	image_type type;
	VFile *f;

	// FIXME: instead of return, load a white image.
	if(!strcmp(tr->name, "noname")) return;
	if(!strcmp(tr->name, "$lightmap")) return;
	if(!strcmp(tr->name, "$whiteimage")) return;


	// load file
	type=IT_TGA;
	strcat(tr->name, ".tga");
	f=new VFile(tr->name);
	if(!f->mem){					// FIXME: better error testing.
		delete f;
		f_StripExtension(tr->name);
		type=IT_JPG;
		strcat(tr->name, ".jpg");
		f=new VFile(tr->name);
		if(!f->mem){
			delete f;
			f_StripExtension(tr->name);
			type=IT_WAL;
			strcat(tr->name, ".wal");
			f=new VFile(tr->name);
		}
	}
	
	// and decode image.
	if(f->mem){

		tr->info->mem=NULL;

		switch(type){
			case IT_TGA:
				TGA_Decode(f, tr->info);
			break;
			case IT_JPG:
				JPG_Decode(f, tr->info);
			break;
			case IT_WAL:
				WAL_Decode(f, tr->info);
			break;
		};

		if( tr->info->mem==NULL ) {
			err->Log(UNKNOWN_ERROR, "Error Decoding File %s, loading default file instead...\n", tr->name);
			LoadDefaultTex(tr->info);
			delete f;
			return;
		}

		if(!cvars->ValueForKey("r_overBrightBits")){
			AdjustColor(tr->info, 2.0f, 1.0f, 0.0f);
		}

		render->RegisterTexInfo(tr->info, tr->flags);

		free(tr->info->mem);
		tr->info->mem=NULL;

		delete f;
		return;
	}
	

	err->Log(ERROR_LOADING_FILE, "Coudn't load %s\n", tr->name);
	LoadDefaultTex(tr->info);
	
	delete f;
	return;
}







/*---------------------------------------------------------------------------
	Private Methods:
---------------------------------------------------------------------------*/


//
// Load a tga
//
void TexMng::TGA_Decode(VFile *vf, texinfo *tex) {
	int columns, rows, numPixels;
	int row, column;
	byte *rgba, *dest;
	TgaHeader *header;
	byte red,green,blue,alpha,packetHeader,packetSize,j;
	byte *tga=vf->mem;

	header = (TgaHeader *)tga;
	tga+=sizeof(TgaHeader);


	// Only types 2 and 10 are supported
	if (header->image_type!=2 && header->image_type!=10) {
		tex->mem=NULL;
		return;
	}

	// Only 24bit or 32bit pixels are supported
	if (header->colormap_type!=0||
		(header->pixel_size!=32&&header->pixel_size!=24)) {
		tex->mem=NULL;
		return;
	}

	columns = header->width;
	rows = header->height;
	numPixels = columns * rows;
	header->pixel_size==32?tex->bpp=4:tex->bpp=3;

	tex->width=columns;
	tex->height=rows;

	// Allocate memory for decoded image
	rgba = (byte *)malloc(numPixels*(tex->bpp));
	if (!rgba) {
		tex->mem=NULL;
		return;
	}

	
	// Uncompressed RGB image
	if (header->image_type==2) {
		for(row=rows-1; row>=0; row--) {
			dest = rgba + row*columns*(tex->bpp);
			for(column=0; column<columns; column++) {
				if (header->pixel_size==24) {
					blue = *tga; tga++;
					green = *tga; tga++;
					red = *tga; tga++;
					*dest++ = red;
					*dest++ = green;
					*dest++ = blue;
					//*dest++ = 255;
				}
				else if (header->pixel_size==32) {
					blue = *tga; tga++;
					green = *tga; tga++;
					red = *tga; tga++;
					alpha = *tga; tga++;
					*dest++ = red;
					*dest++ = green;
					*dest++ = blue;
					*dest++ = alpha;
				}
			}
		}
	}
// RLE RGB image
	else if (header->image_type==10) {
		for(row=rows-1; row>=0; row--) {
			dest = rgba + row*columns*(tex->bpp);
			for(column=0; column<columns; ) {
				packetHeader=*tga; tga++;
				packetSize = 1 + (packetHeader & 0x7f);
			// RLE packet
				if (packetHeader & 0x80) {
					if (header->pixel_size==24) {
						blue = *tga; tga++;
						green = *tga; tga++;
						red = *tga; tga++;
						//alpha = 255;
					}
					else if (header->pixel_size==32) {
						blue = *tga; tga++;
						green = *tga; tga++;
						red = *tga; tga++;
						alpha = *tga; tga++;
					}
	
					for(j=0;j<packetSize;j++) {
						*dest++=red;
						*dest++=green;
						*dest++=blue;
						if(header->pixel_size==32) *dest++=alpha;
						column++;
						if (column==columns) {
							column=0;
							if (row>0)
								row--;
							else
								goto end_decode;
							dest = rgba + row*columns*(tex->bpp);
						}
					}
				}
			// Non-RLE packet
				else {
					for(j=0;j<packetSize;j++) {
						if (header->pixel_size==24) {
							blue = *tga; tga++;
							green = *tga; tga++;
							red = *tga; tga++;
							*dest++ = red;
							*dest++ = green;
							*dest++ = blue;
							//*dest++ = 255;
						}
						else {
							blue = *tga; tga++;
							green = *tga; tga++;
							red = *tga; tga++;
							alpha = *tga; tga++;
							*dest++ = red;
							*dest++ = green;
							*dest++ = blue;
							*dest++ = alpha;
						}
						column++;
						if (column==columns) {
							column=0;
							if (row>0)
								row--;
							else
								goto end_decode;
							dest = rgba + row*columns*(tex->bpp);
						}						
					}
				}
			}
			end_decode:;
		}
	}	
	
	tex->mem=rgba;
	return;
}



#ifndef WIN32
//
// Load a jpeg using jpeglib
//

// This procedure is called by the IJPEG library when an error
// occurs.
static void error_exit (j_common_ptr pcinfo){
	// Create the message string
	char sz[256];
	(pcinfo->err->format_message) (pcinfo, sz);

	err->CriticalError(ERROR_FORMAT_NOT_SUPPORTED, "%s\n",sz);
}


static void init_source (j_decompress_ptr cinfo){
}


static boolean fill_input_buffer (j_decompress_ptr cinfo){
	struct jpeg_source_mgr * src = cinfo->src;
	static JOCTET FakeEOI[] = { 0xFF, JPEG_EOI };

	/* Generate warning */
	err->Log(UNKNOWN_ERROR, "Premature end of file\n");
  
	/* Insert a fake EOI marker */
	src->next_input_byte = FakeEOI;
	src->bytes_in_buffer = 2;

	return TRUE;
}


static void skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	struct jpeg_source_mgr * src = cinfo->src;
  
	if(num_bytes >= (long)src->bytes_in_buffer) {
		fill_input_buffer(cinfo);
		return;
	}

	src->bytes_in_buffer -= num_bytes;
	src->next_input_byte += num_bytes;
}


void term_source (j_decompress_ptr cinfo){
  /* no work necessary here */
}


void TexMng::JPG_Decode(VFile *vf, texinfo *tex){
	jpeg_decompress_struct cinfo;	// IJPEG decoder state.
	jpeg_error_mgr         jerr;	// Custom error manager.

	cinfo.err = jpeg_std_error (&jerr);
	jerr.error_exit = error_exit;	// Register custom error manager.

	jpeg_create_decompress (&cinfo);

	
	struct jpeg_source_mgr * src;
	//jpeg_stdio_src(&cinfo, fp);
	
	cinfo.src= (struct jpeg_source_mgr *) (*cinfo.mem->alloc_small) 
		((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(struct jpeg_source_mgr));

	
	src = cinfo.src;

	src->init_source = init_source;
	src->fill_input_buffer = fill_input_buffer;
	src->skip_input_data = skip_input_data;
	src->resync_to_restart = jpeg_resync_to_restart;	// use default method
	src->term_source = term_source;

	// Set up data pointer
	src->bytes_in_buffer = vf->size;
	src->next_input_byte = vf->mem;

	jpeg_read_header (&cinfo, TRUE);

	cinfo.do_fancy_upsampling = FALSE;		// fast decompression

    cinfo.dct_method = JDCT_FLOAT;			// Choose floating point DCT method.



	jpeg_start_decompress(&cinfo);

    tex->width = cinfo.output_width;
    tex->height = cinfo.output_height;

	if (cinfo.out_color_space == JCS_GRAYSCALE){
		/*tex->bpp=1;
		tex->mem = (byte *) malloc(1*tex->width*tex->height);
		if (!tex->mem) {
			tex->mem=NULL;
			return;
		}*/
		err->CriticalError(UNKNOWN_ERROR,"grayscale not supported!!!");
	}
	else{
		tex->bpp=3;
		tex->mem = (byte *) malloc(3*tex->width*tex->height);
		if (!tex->mem) {
			return;
		}
		
		byte *pDst=tex->mem;
		byte **ppDst=&pDst;
		int num_scanlines=0;
		while (cinfo.output_scanline < cinfo.output_height){
			num_scanlines=jpeg_read_scanlines (&cinfo, ppDst, 1);
			*ppDst += num_scanlines * 3 * cinfo.output_width;
		}
	}
	
	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress (&cinfo);
}
#else


//
// Load a jpeg using ijl
//
void TexMng::JPG_Decode(VFile *vf, texinfo *tex){
	JPEG_CORE_PROPERTIES jcprop;
	
	appMemzero( &jcprop, sizeof( JPEG_CORE_PROPERTIES ) );

	if( ijlInit( &jcprop ) != IJL_OK ) {
		err->Log(ERROR_LOADING_MODULE, "Can't initialize intel jpeg library!\n");
		return;
	}


	jcprop.JPGBytes = vf->mem;
	jcprop.JPGSizeBytes = vf->size;
	if( ijlRead( &jcprop, IJL_JBUFF_READPARAMS ) != IJL_OK ){
		err->Log(ERROR_READING_FILE, "Can't read jpeg header\n");
		return;
	}

	tex->width=jcprop.JPGWidth;
	tex->height=jcprop.JPGHeight;
	tex->bpp=jcprop.JPGChannels;

	int imageSize = jcprop.JPGWidth*jcprop.JPGHeight*jcprop.JPGChannels;


	tex->mem = (byte *) malloc(imageSize);
	if (!tex->mem) {
		err->Log(ERROR_ALLOCATING_MEMORY, "Error Allocating memory for textures!\n");
		return;
	}

	jcprop.DIBWidth    = jcprop.JPGWidth;
	jcprop.DIBHeight   = jcprop.JPGHeight;
	jcprop.DIBChannels = jcprop.JPGChannels;
	jcprop.DIBBytes    = tex->mem;
	
	switch(jcprop.JPGChannels){
		case 1:
			jcprop.DIBColor	   = IJL_G;
		break;
		default:
		case 3:
			jcprop.DIBColor	   = IJL_RGB;
		break;
	}
	

	if( ijlRead( &jcprop, IJL_JBUFF_READWHOLEIMAGE ) != IJL_OK ){
		err->Log(ERROR_READING_FILE, "Can't read jpeg file\n");
		free(tex->mem);
		tex->mem=NULL;
		return;
	}

	if( ijlFree( &jcprop ) != IJL_OK ) {
		err->Log(ERROR_FREEING_MODULE, "Can't free intel jpeg library!\n");
		return;
	}
}
#endif

//
// Load a wal
//
void TexMng::WAL_Decode(VFile *vf, texinfo *tex) {
	miptex_t *header;
	byte *mur=vf->mem;

	header = (miptex_t *)mur;
	mur+=sizeof(miptex_t);

	tex->width = header->width;
	tex->height= header->height;
	tex->bpp=3;

	// Allocate memory for decoded image
	tex->mem = (byte *) malloc(3*tex->width*tex->height);
	if (!tex->mem) {
		tex->mem=NULL;
		return;
	}

	for(int i=0;i<tex->width*tex->height;i++){
		tex->mem[3*i]=pal[3*mur[i]];
		tex->mem[3*i+1]=pal[3*mur[i]+1];
		tex->mem[3*i+2]=pal[3*mur[i]+2];
	}
}


//
// Loads the default texture
//
// TODO: use a better default texture, this looks jerky
void TexMng::LoadDefaultTex(texinfo *t){
	t->mem=(byte *)malloc(3*4);
	memset(t->mem,0,12);
	t->mem[11]=255;
	t->bpp=3;
	t->height=2;
	t->width=2;

	render->RegisterTexInfo(t, LF_NOMIPMAP|LF_NOPICMIP);

	free(t->mem);
	t->mem=NULL;

}






/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/


//
// Adjust color, currently only overbright is used
//
void AdjustColor(texinfo *t, float overbright, float gamma, float lightness){
	int j;
	BYTE *c;
	float r, g, b;

	c = t->mem;

	for (j=0; j < t->width*t->height; j++, c+=t->bpp){
		float scale=1.0f;
		float tmp_scale;

		r = (float) c[0];
		g = (float) c[1];
		b = (float) c[2];
		r = r * overbright / 255.0f;
		g = g * overbright / 255.0f;
		b = b * overbright / 255.0f;
				
		if(r>1.0f && (tmp_scale=(1.0f/r))<scale) scale=tmp_scale;
		if(g>1.0f && (tmp_scale=(1.0f/g))<scale) scale=tmp_scale;
		if(b>1.0f && (tmp_scale=(1.0f/b))<scale) scale=tmp_scale;

		scale *= 255.0f;
		r *= scale;
		g *= scale;
		b *= scale;

		c[0] = appRound(r);
		c[1] = appRound(g);
		c[2] = appRound(b);
	}
}
