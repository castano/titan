/*===========================================================================
	Title: Render.h
	Author: Ignacio Castaño Aguado
	Description: Render exported object
	Classes:
		Render		- hmmm
	TODO:
		- lot of things...
===========================================================================*/

#include "render.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

Framework Import;
HGLRC Driver::global_rc=NULL;
PFNGLLOCKARRAYSEXTPROC				glLockArraysEXT;
PFNGLUNLOCKARRAYSEXTPROC			glUnlockArraysEXT;

PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;

PFNWGLSWAPINTERVALPROC				wglSwapIntervalEXT;
PFNWGLGETSWAPINTERVALPROC			wglGetSwapIntervalEXT;

Var		gl_acceleration("gl_acceleration", "Unknown", VF_SYSTEM);
Var		gl_vendor("gl_vendor", "", VF_SYSTEM);
Var		gl_renderer("gl_renderer", "", VF_SYSTEM);
Var		gl_version("gl_version", "", VF_SYSTEM);
Var		gl_extensions("gl_extensions", "", VF_SYSTEM);
Var		gl_maxTextureSize("gl_maxTextureSize", 0, VF_SYSTEM);
Var		gl_maxTextureUnits("gl_maxTextureUnits", 0, VF_SYSTEM);
Var		gl_dither("gl_dither", 0, VF_PERSISTENT);
Var		r_ext_compiledVertexArray("r_ext_compiled_vertex_array", 0, VF_PERSISTENT);
Var		r_ext_multitexture("r_ext_multitexture", 0, VF_PERSISTENT);
Var		r_ext_swapInterval("r_ext_swapInterval", 0, VF_PERSISTENT);
Var		r_vertexLight("r_vertexLight", 0, VF_PERSISTENT);
Var		r_clear("r_clear", 0, VF_PERSISTENT);
Var		r_quality("r_quality", 2, VF_PERSISTENT);
Var		r_grid("r_grid", 0, VF_PERSISTENT);
Var		r_filter("r_filter", 1, VF_PERSISTENT);


/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

// FIXME: BUFFER_SIZE should be allways the limiting factor, so resize MAX_ELEMS or MAX_SURFS if necesary
#define BUFFER_SIZE		1024
#define MAX_ELEMS		4096
#define MAX_SURFS		512		// hmmm very high value...



typedef struct {
	int		st_content;
	float	st[2*BUFFER_SIZE];
	int		c_content;
	int		c[BUFFER_SIZE];
}Buffer;

typedef struct {
    int		numelems;
    int		elems[MAX_ELEMS];

	int		numverts;
	float	xyz[4*BUFFER_SIZE];
	float	tex_st[2*BUFFER_SIZE];
	float	lm_st[2*BUFFER_SIZE];
	float	normal[4*BUFFER_SIZE];
	int		c[BUFFER_SIZE];

}Input;



// buffer states
enum {
	BF_EMPTY,
	BF_FILLING,
	BF_LOCKED
};





/*---------------------------------------------------------------------------
	Render:
---------------------------------------------------------------------------*/

//
// Render exported object
//
class Render : public IRender {
private:
	Driver *		driver;
	render_state	curr_rs;

	int				state;
	Input			input;
	Buffer			buffer[MAX_UNITS];

	camera_s *		current_camera;

public:


	//
	//	Initialization
	//
	int	Init(WindowHandle wnd_handle){
		driver=new Driver();
		
		if(!driver->init()) {
			delete driver;
			return 0;
		}

		Import.console->Insert("--- gl_acceleration: %s\n", Import.cvars->StringForKey("gl_acceleration"));
		Import.console->Insert("--- gl_vendor: %s\n", Import.cvars->StringForKey("gl_vendor"));
		Import.console->Insert("--- gl_renderer: %s\n", Import.cvars->StringForKey("gl_renderer"));
		Import.console->Insert("--- gl_version: %s\n", Import.cvars->StringForKey("gl_version"));
		Import.console->Insert("--- gl_extensions: %s\n", Import.cvars->StringForKey("gl_extensions"));

		State::SetInitialState(driver->ext_multitexture);

		Import.console->Insert("--- gl_max_texture_size: %s\n", Import.cvars->StringForKey("gl_maxTextureSize"));
		Import.console->Insert("--- gl_max_texture_units: %s\n", Import.cvars->StringForKey("gl_maxTextureUnits"));



		//_controlfp(_PC_24, _MCW_PC);
		state=BF_EMPTY;
		input.numverts=0;
		input.numelems=0;
		
		return 1;
	};

	
	//
	//	Finalization
	//
	void Shut(){
		glDepthMask(GL_TRUE);								// must be true before clearing!
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		driver->shut();
		delete driver;
	};


	//
	// Render info
	//
	void QueryInfo(render_info *ri){
		ri->current_driver=0;
		ri->num_drivers=1;
		strcpy(ri->driver_names[0], "opengl32.dll");
	};


	//
	// Changes state
	//
	void SetState(render_state *rs){
		curr_rs.mode=rs->mode;
		curr_rs.show_triangles=rs->show_triangles;
		curr_rs.use_extensions=rs->use_extensions;
	};



	//
	//	Lock the render
	//
	void Lock(void){
		driver->Lock();
	};


	//
	//	Unlock the render
	//
	void Unlock(void){
		driver->Unlock();
	};


	//
	//	Begin Frame
	//
	int	BeginFrame(){
		glDepthMask(GL_TRUE);								// must be true before clearing!
		
		if(r_clear.ivalue)
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		else 
			glClear(GL_DEPTH_BUFFER_BIT);

		state=BF_EMPTY;
		input.numverts=0;
		input.numelems=0;

		return 1;
	};

	
	//
	//	Ends Frame
	//
	int	EndFrame(){
		// flush all the buffers
		if(state!=BF_EMPTY) flush();

		driver->SwapBuffers();
		State::curr_shader=NULL;

		return 1;
	};


	//
	//	Register Texinfo by loading the texture into video memory
	//
	int RegisterTexInfo(texinfo *tex, int flags){
		int picmip = (flags&LF_NOPICMIP)?0:appRound(Import.cvars->ValueForKey("r_picmip"));
		int newwidth=(1<<Log2(tex->width))>>picmip;
		int newheight=(1<<Log2(tex->height))>>picmip;
		int format, outformat;
		float fform;
		byte *mem=tex->mem;

		if(tex->num!=~0) return 1;		// FIXME: ¿1?
		
		switch(tex->bpp){
			case 1:
				format=GL_ALPHA;
				outformat=GL_ALPHA;
			break;
			case 2:
				format=GL_LUMINANCE_ALPHA;		// do I support that??
				outformat=GL_LUMINANCE_ALPHA;
			break;
			case 3:
				format=GL_RGB;
				fform=Import.cvars->ValueForKey("r_texturebits");
				switch((int)fform){				// FIXME: float to int conversion !!!
					case 16:
						outformat=GL_RGB5;
						break;
					case 32:
						outformat=GL_RGB8;
						break;
					default:
						outformat=GL_RGB;
				};
			break;
			case 4:
				format=GL_RGBA;
				fform=Import.cvars->ValueForKey("r_texturebits");
				switch((int)fform){				// FIXME: float to int conversion !!!
					case 16:
						outformat=GL_RGBA4;
						break;
					case 32:
						outformat=GL_RGBA8;
						break;
					default:
						outformat=GL_RGBA;
				};
			break;
			default:
				format = GL_RGB;
				outformat = GL_RGB;
		}
		
		if(newheight>gl_maxTextureSize.ivalue)newheight=gl_maxTextureSize.ivalue;
		if(newwidth>gl_maxTextureSize.ivalue)newwidth=gl_maxTextureSize.ivalue;

		if(tex->height!=newheight||tex->width!=newwidth){
			mem = (byte *) malloc(newheight*newwidth*tex->bpp);
			ScaleImage(format, tex->width, tex->height, tex->mem, newwidth, newheight, mem);
		}

		glGenTextures(1, &tex->num);
		glBindTexture(GL_TEXTURE_2D, tex->num);

		if(flags&LF_NOMIPMAP)
			glTexImage2D(GL_TEXTURE_2D, 0, outformat, newwidth, newheight, 0, format, GL_UNSIGNED_BYTE, mem);
		else
			BuildMipmaps(outformat, newwidth, newheight, format, mem);

		if(flags&LF_CLAMP){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		
		//float filter = Import.cvars->ValueForKey("gl_filter");
		
		if(r_filter.ivalue==0){	// nearest
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			if(flags&LF_NOMIPMAP)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		else if(r_filter.ivalue==2){	// trilinear
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if(flags&LF_NOMIPMAP)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else /*if(r_filter.ivalue==1)*/{		// bilinear
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if(flags&LF_NOMIPMAP)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}


		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		if(mem!=tex->mem) free(mem);
		return 1;
	};


	//
	//	Deletes texture memory
	//
	void UnregisterTexInfo(texinfo *tex){
		glDeleteTextures(1, &tex->num);
	};


	//	
	//	Takes a screenshot (reads a portion of the backbuffer)
	//
	// NOTE: buffer should be already allocated
	void Screenshot(byte *buffer, int left, int top, int right, int bottom){
		glFlush();
		glReadPixels(left, top, right, bottom, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	};


	//
	//	Sets the given render state
	//
	void SetRenderState(Shader *s, int lightmap){
		if(state!=BF_EMPTY) flush();

		State::curr_shader=s;
		State::curr_lightmap=lightmap;

		if(!s || s->surface_flags&SURF_SKIP || s->surface_flags&SURF_NODRAW) State::skipping=true;
		else State::skipping=false;
	};


	//
	//	Set current drawing viewport
	//
	void SetViewPort(camera_s *cam, int type){

		if(state!=BF_EMPTY) flush();

		if(type){
			InitCamera(cam);
		}
		else {
			glViewport( 0, 0, cam->width, cam->height);
			glScissor(0, 0, cam->width, cam->height);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			//glOrtho( 0, 640, 480, 0, -1.0, 1.0 );
			glOrtho( 0, 640, 480, 0, 0.0f, 1.0f );
	
			if(State::curr_depthFunc!=GL_LEQUAL) glDepthFunc(GL_LEQUAL);
		}
	}



	//
	//	Write vertexes in the input, flush when full
	//
	void PushTriangles(Surface *s){

		if(State::skipping) return;

		if(s->numverts>BUFFER_SIZE) 
			return;	// FIXME: what happens if that happens ??? // THAT HAPPENS ON HIGHLY TESSELATED CURVES !!!
		if(s->numelems>MAX_ELEMS) 
			return;		// ???
		
		
		if(s->numelems==0 || s->numverts==0) return;

		// test array bounds
		if( input.numelems + s->numelems >= MAX_ELEMS || 
			input.numverts + s->numverts >= BUFFER_SIZE ) {
			
			flush();
		}

		state=BF_FILLING;
		
		for(int i=0; i<s->numelems; i++){
			input.elems[input.numelems] = input.numverts + s->firstelem[i];
			input.numelems++;
		}

		float *	xyz;
		float *	st;
		vertex_t *v;

		xyz=&input.xyz[input.numverts<<2];
		v=s->firstvert;
		i=s->numverts;
		do {
			VectorCopy(v->v_point, xyz);
			xyz+=4;
			++v;
		}while(--i);

		st=&input.tex_st[input.numverts<<1];
		v=s->firstvert;
		i=s->numverts;
		do {
			TexcoordCopy(v->tex_st, st);
			st+=2;
			++v;
		}while(--i);

		
		if(State::curr_shader->flags&SF_HASLIGHTMAP){
			st=&input.lm_st[input.numverts<<1];
			v=s->firstvert;
			i=s->numverts;
			do {
				TexcoordCopy(v->lm_st, st);
				st+=2;
				++v;
			}while(--i);
		}

		if(State::curr_shader->flags&SF_USENORMALS){
			float *n=&input.normal[input.numverts<<2];
			v=s->firstvert;
			i=s->numverts;
			do {
				VectorCopy(v->v_norm, n);
				n+=4;
				++v;
			}while(--i);
		}

		if(State::curr_shader->flags&SF_HASCOLORS){
			int *c=&input.c[input.numverts];
			v=s->firstvert;
			i=s->numverts;
			do {
				*c++	= v->colour;
				++v;
			}while(--i);
		}


		input.numverts+=s->numverts;
		
	}


private:

	//
	//	flush a buffer setting the aproppiate state
	//
	void flush(void){
		Shader *s=State::curr_shader;
	
		if(input.numelems==0 || s->num_layers==0) return;
		if(State::skipping) return;

		// set buffer states:
		state=BF_LOCKED;
		buffer[0].st_content=-1;		// the buffers are allways dirty the first time.
		buffer[1].st_content=-1;
		buffer[0].c_content=-1;
		buffer[1].c_content=-1;


		// set culling:
		if(s->cull!=State::curr_culling){
			if(s->cull){
				glEnable(GL_CULL_FACE);
				if(s->cull<0) glCullFace(GL_FRONT);
				else glCullFace(GL_BACK);
			}
			else {
				glDisable(GL_CULL_FACE);
			}
			State::curr_culling=s->cull;
		}

		// select render path
		if(r_vertexLight.ivalue){
			flushVertexLit();
		}
		else if(driver->ext_compiledVertexArray){
			if(driver->ext_multitexture && s->flags&SF_MULTITEX){
				flushMtxCVA();
			}
			else if(s->num_layers==1){
				flushSingleCVA();
			}
			else {
				flushGenericCVA();
			}
		}
		else {
			if(driver->ext_multitexture && s->flags&SF_MULTITEX){
				flushMtx();
			}
			else if(s->num_layers==1){
				flushSingle();
			}
			else {
				flushGeneric();
			}
		}

		// clear buffer state:
		state=BF_EMPTY;
		input.numelems=0;
		input.numverts=0;

	}


	// flush multitexture faces using CVAs
	void flushMtxCVA(void){
		
		SetVertexCoords();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	// dont allow to cache this!
		glDisableClientState(GL_COLOR_ARRAY);
		glLockArraysEXT(0, input.numverts);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		
		
		for(State::pass=0; State::pass<State::curr_shader->num_layers; State::pass++){
			Layer *l=State::curr_shader->layer[State::pass];

			SetTextureCoordinates(0);

			SetVertexColors();

			State::BindTexture(l, 0);
			State::setState(l);
			if(l->flags&LF_MULTITEX){

				State::pass++;
				l=State::curr_shader->layer[State::pass];
				
				
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glEnable(GL_TEXTURE_2D);
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
				
				SetTextureCoordinates(1);

				State::BindTexture(l, 1);
				
				glDrawElements( GL_TRIANGLES, input.numelems, GL_UNSIGNED_INT, input.elems);
				
				glDisable(GL_TEXTURE_2D);
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glClientActiveTextureARB(GL_TEXTURE0_ARB);
			}
			else {
				glDrawElements( GL_TRIANGLES, input.numelems, GL_UNSIGNED_INT, input.elems);
			}
		}
		glUnlockArraysEXT();
	};

	// flush multitexture faces with lightmap
	void flushMtx(void){

		SetVertexCoords();
		
		for(State::pass=0; State::pass<State::curr_shader->num_layers; State::pass++){
			Layer *l=State::curr_shader->layer[State::pass];
		
			SetTextureCoordinates(0);

			SetVertexColors();

			State::BindTexture(l, 0);
			State::setState(l);
			if(l->flags&LF_MULTITEX){

				State::pass++;
				l=State::curr_shader->layer[State::pass];
				
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glEnable(GL_TEXTURE_2D);
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
				
				SetTextureCoordinates(1);

				State::BindTexture(l, 1);
				
				glDrawElements( GL_TRIANGLES, input.numelems, GL_UNSIGNED_INT, input.elems);
				
				glDisable(GL_TEXTURE_2D);
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glClientActiveTextureARB(GL_TEXTURE0_ARB);
			}
			else {
				stripmine(input.numelems, input.elems);
			}
		}
	};


	// flush faces without lightmap using CVAs
	void flushGenericCVA(void){
		SetVertexCoords();		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	// dont allow to cache this!
		glDisableClientState(GL_COLOR_ARRAY);
		glLockArraysEXT(0, input.numverts);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		for(State::pass=0; State::pass<State::curr_shader->num_layers; State::pass++){
			Layer *l=State::curr_shader->layer[State::pass];

			SetTextureCoordinates(0);

			SetVertexColors();

			State::BindTexture(l, 0);
			State::setState(l);
			glDrawElements( GL_TRIANGLES, input.numelems, GL_UNSIGNED_INT, input.elems);
		}
		glUnlockArraysEXT();
	};


	// flush faces without lightmap
	void flushGeneric(void){
		SetVertexCoords();

		for(State::pass=0; State::pass<State::curr_shader->num_layers; State::pass++){
			Layer *l=State::curr_shader->layer[State::pass];

			SetTextureCoordinates(0);
			SetVertexColors();

			State::BindTexture(l, 0);
			State::setState(l);
			stripmine(input.numelems, input.elems);
		}
	};


	// flush faces with a single layer using CVAs
	void flushSingleCVA(void){
		SetVertexCoords();

		State::pass=0;
		Layer *l=State::curr_shader->layer[0];

		SetTextureCoordinates(0);

		SetVertexColors();

		glLockArraysEXT(0, input.numverts);
		State::BindTexture(l, 0);
		State::setState(l);
		glDrawElements( GL_TRIANGLES, input.numelems, GL_UNSIGNED_INT, input.elems);

		glUnlockArraysEXT();
	};


	// flush faces with a single layer without using CVAs
	void flushSingle(void){
		SetVertexCoords();
		State::pass=0;
		Layer *l=State::curr_shader->layer[0];
		
		SetTextureCoordinates(0);
		SetVertexColors();

		State::BindTexture(l, 0);
		State::setState(l);
		stripmine(input.numelems, input.elems);
	};


	// FIXME: this is only experimental
	void flushVertexLit(void){
		Layer *l;

		SetVertexCoords();

		State::pass=0;
		if(State::curr_shader->layer[0]->flags&LF_LIGHTMAP){
			State::pass++;
			l=State::curr_shader->layer[State::pass];

			SetTextureCoordinates(0);
		
////		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &input.c);

			State::BindTexture(l, 0);
			
			if(State::curr_blendsrc!=GL_ONE || State::curr_blenddst!=GL_ZERO){
				glDisable(GL_BLEND);
				State::curr_blenddst=l->blenddst;
				State::curr_blendsrc=l->blendsrc;
			}
			
			stripmine(input.numelems, input.elems);
			State::pass++;
		}

		for(; State::pass<State::curr_shader->num_layers; State::pass++){
			l=State::curr_shader->layer[State::pass];
			if(l->flags&LF_LIGHTMAP) continue;

			SetTextureCoordinates(0);
			SetVertexColors();
		
			State::BindTexture(l, 0);
			State::setState(l);
			stripmine(input.numelems, input.elems);
		}

	};



	// that have been taken from aftershock:	(FIXME: can I use it?)
	void stripmine(int numelems, int *elems){
		int toggle;
		int a, b, elem;

		/* Vertexes are in tristrip order where possible.  If we can't lock
		 * the vertex arrays (glLockArraysEXT), then it's better to send
		 * tristrips instead of triangles (less transformations).
		 * This function looks for and sends tristrips.
		 */

		/* Tristrip order elems look like this:
		 *  0 1 2 2 1 3 2 3 4 4 3 5 4 5 7 7 5 6  <-- elems
		 *    b a a b b a b a a b b a b a a b b  <-- ab pattern
		 *    \ 1 / \ 2 / \ 3 / \ 4 / \ 5 /      <-- baa/bba groups
		 */
    
		elem = 0;
		while (elem < numelems){
			toggle = 1;
			glBegin(GL_TRIANGLE_STRIP);
		
			glArrayElement(elems[elem++]);
			b = elems[elem++];
			glArrayElement(b);
			a = elems[elem++];
			glArrayElement(a);
	
			while (elem < numelems){
				if (a != elems[elem] || b != elems[elem+1]) break;
				if (toggle) {
					b = elems[elem+2];
					glArrayElement(b);
				}
				else {
					a = elems[elem+2];
					glArrayElement(a);
				}
				elem += 3;
				toggle = !toggle;
			}
			glEnd();
		}
	}

	
	void SetVertexCoords(void){
		glVertexPointer(3, GL_FLOAT, 16, input.xyz);

		switch(State::curr_shader->deformVertexes){
			/*case DEFORMVERTEXES_WAVE:{
				//float p[6];
				float inc;
				for(int i=0; i<input.numverts; i++){
					inc = State::Eval(&State::curr_shader->deformVertexes_params[1]);
					input.xyz[i*4+2]+=inc*10;
				}
			}break;*/
		}
		
	}


	// TODO: save last tcgen and last tcmod to know if we dont have to recopy the coordinates
	void SetTextureCoordinates(int tmu){
		Layer *l=State::curr_shader->layer[State::pass];

		glTexCoordPointer(2, GL_FLOAT, 0, buffer[tmu].st);
		
		// if the contents are the same, and the buffer isnt dirty dont fill it again.
		if(buffer[tmu].st_content!=l->tcGen || buffer[tmu].st_content==-1) {

			buffer[tmu].st_content=l->tcGen;

			switch(l->tcGen){
				case TCGEN_LIGHTMAP:{
					/*for(int i=0; i<input.numverts; i++){
						buffer[tmu].st[i*2+0]=input.lm_st[i*2+0];
						buffer[tmu].st[i*2+1]=input.lm_st[i*2+1];
					}*/
					appMemcpy(buffer[tmu].st, input.lm_st, sizeof(float)*2*input.numverts);
				}break;
				case TCGEN_ENVIRONMENT:{
					vec3_t dir;
					for(int i=0; i<input.numverts; i++){
						VectorSub(&input.xyz[i*4], current_camera->c, dir);

						FastNormVect3(dir);

						dir[0]+=input.normal[i*4+0];
						dir[1]+=input.normal[i*4+1];

						buffer[tmu].st[i*2+0]=dir[0];
						buffer[tmu].st[i*2+1]=dir[1];
					}

				}break;
				case TCGEN_BASE:{
				case TCGEN_VECTOR:
				default:
					/*for(int i=0; i<input.numverts; i++){
						buffer[tmu].st[i*2+0]=input.tex_st[i*2+0];
						buffer[tmu].st[i*2+1]=input.tex_st[i*2+1];
					}*/
					appMemcpy(buffer[tmu].st, input.tex_st, sizeof(float)*2*input.numverts);
				}break;
			}
		}
		
		if(l->tcMod) {
			buffer[tmu].st_content=-1;	// texcoord transforms always destroy the content

			for(int i=0; i<input.numverts; i++){
				float *st=&buffer[tmu].st[i*2];

				for(funclist *f=l->tcMod; f; f=f->next){
					switch(f->func){
						case TCMOD_ROTATE:{
							float angulo=f->p[0]*Import.timer->fTime/180*3.14159;
							float cost=FastCos(angulo);
							float sint=FastSin(angulo);
							vec2_t v;
							TexcoordCopy(st, v);
							v[0] -= .5f;
							v[1] -= .5f;
							st[0] = v[0]*cost - v[1]*sint + 0.5f;
							st[1] = v[0]*sint + v[1]*cost + 0.5f;
						}break;
						case TCMOD_SCALE:
							st[0] = (st[0]-0.5f) * f->p[0] + 0.5f;
							st[1] = (st[1]-0.5f) * f->p[1] + 0.5f;
						break;
						case TCMOD_SCROLL:{
							vec2_t scroll;
							scroll[0] = f->p[0]*Import.timer->fTime;
							scroll[1] = f->p[1]*Import.timer->fTime;
							//scroll[0] -= floor(scroll[0]);
							//scroll[1] -= floor(scroll[1]);
							st[0] += scroll[0];
							st[1] += scroll[1];
						}break;
						case TCMOD_STRETCH:{
							float factor=1/State::Eval(f->p);
							st[0] = (st[0] - 0.5f) * factor + 0.5f;
							st[1] = (st[1] - 0.5f) * factor + 0.5f;
						}break;
						case TCMOD_TRANSFORM:{
							vec2_t v;
							TexcoordCopy(st, v);
							st[0] = v[0]*f->p[0] + v[1]*f->p[2] + f->p[4];
							st[1] = v[0]*f->p[1] + v[1]*f->p[3] + f->p[5];
						}break;
						case TCMOD_TURB:{
							float *v=&input.xyz[i*4];
							st[0] += FastSin((v[0]+v[2])*0.1f*f->p[3] + Import.timer->fTime + f->p[2]) * f->p[1] * 0.5f;
							st[1] += FastSin((v[1]     )*0.1f*f->p[3] + Import.timer->fTime + f->p[2]) * f->p[1] * 0.5f;
						}break;
					}
				}

			}

		}
	}


	void SetVertexColors(void){
		Layer *l=State::curr_shader->layer[State::pass];
		int color;

		BYTE alpha=255;

		glColorPointer(4, GL_UNSIGNED_BYTE, 0, buffer[0].c);

		if(buffer[0].c_content==-1 || buffer[0].c_content!=l->rgbGen) {

			buffer[0].c_content=l->rgbGen;

			switch(l->rgbGen){
				case  RGBGEN_IDENTITY:{
					color=(alpha<<24)|(255<<16)|(255<<8)|(255);
					for(int i=0; i<4*input.numverts; i++)
						buffer[0].c[i]=color;
				}break;
				case RGBGEN_IDENTITY_LIGHTING:{
					color=(alpha<<24)|(127<<16)|(127<<8)|(127);
					for(int i=0; i<4*input.numverts; i++)
						buffer[0].c[i]=color;
				}break;
				case RGBGEN_VERTEX:{
					//for(int i=0; i<input.numverts; i++){
					//	buffer[0].c[i]=input.c[i];
					//}
					appMemcpy(buffer[0].c, input.c, 4*input.numverts);

				}break;
				case RGBGEN_LIGHTING_DIFFUSE:
					//appMemcpy(buffer[0].c, input.c, 4*input.numverts);
				break;
				case RGBGEN_WAVE:
					int c= appRound(255.0f*State::Eval(l->rgbGenParams));
					if(c<0)c=0;
					if(c>255)c=255;
					color=(alpha<<24)|(c<<16)|(c<<8)|(c);
					buffer[0].c_content=-1;
					for(int i=0; i<4*input.numverts; i++)
						buffer[0].c[i]=color;
				break;
			}
		}
	}




	//
	//	Initialize camera
	//
	void InitCamera(camera_s *cam){
		current_camera=cam;
		if(cam){
			float temp[16];
			glViewport(0, 0, cam->width, cam->height);
			glScissor(0, 0, cam->width, cam->height);

			//Import.console->Insert("%d, %d\n", cam->width, cam->height);

			// FIXME: quake3 loads his owns matrices...
			
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			Perspective(cam->fov, cam->proj_ratio, 4.0f, 3000.0f);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			/*
				in q3 +z is up, -z is down
			*/

			glRotatef(-cam->pitch,	1, 0, 0);
			glRotatef(-cam->yaw,	0, 0, 1);
			glRotatef(-cam->roll,	0, 1, 0);
			glTranslatef(-cam->c[0],-cam->c[1],-cam->c[2]);

			// FIXME: do that only if angles have change.
			// Update camera rotation matrix
			glGetFloatv(GL_MODELVIEW_MATRIX, temp);
			for(int i=0;i<3;i++){			// copia matriz 4x4 a 3x3
				for(int j=0;j<3;j++){
					cam->Rotation[i*3+j]=temp[i*4+j];
				}
			}
			vec3_t v={0, 0, -1};
			MultVect3x3(cam->Rotation, v, cam->eyedir);
			cam->eyedist=DotProduct(cam->c, cam->eyedir);

		}
		else {
			// display error!
		}
	};


	// FIXME: Carmack uses vertical fov to deal with 16:9 monitors!
	// TODO: put that in glutils...
	void Perspective( GLdouble fov, GLdouble aspect, GLdouble zNear, GLdouble zFar ){
		GLdouble xmin, xmax, ymin, ymax;
		GLdouble one_deltax, one_deltay, one_deltaz, doubleznear;

		xmax = zNear * tan( fov/2 );
		xmin = -xmax;

		ymax = xmax / aspect;
		ymin = -ymax;

		//glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
		float m[16];

		doubleznear = 2. * zNear;
		one_deltax = 1. / (xmax - xmin);
		one_deltay = 1. / (ymax - ymin);
		one_deltaz = 1. / (zFar - zNear);

		m[0] = (float)(doubleznear * one_deltax);
		m[1] = 0.f;
		m[2] = 0.f;
		m[3] = 0.f;
		m[4] = 0.f;
		m[5] = (float)(doubleznear * one_deltay);
		m[6] = 0.f;
		m[7] = 0.f;
		m[8] = (float)((xmax + xmin) * one_deltax);
		m[9] = (float)((ymax + ymin) * one_deltay);
		m[10] = (float)(-(zFar + zNear) * one_deltaz);
		m[11] = -1.f;
		m[12] = 0.f;
		m[13] = 0.f;
		m[14] = (float)(-(zFar * doubleznear) * one_deltaz);
		m[15] = 0.f;

		glLoadMatrixf(m);
	};

};


//
// Creates a render to export it to the app
//
void __cdecl CreateRender(IRender **ppRender, Framework *rImport){
	*ppRender=(IRender *) new Render();
	memcpy(&Import,rImport, sizeof(Framework));

	Import.cvars->RegisterVar(gl_acceleration);
	Import.cvars->RegisterVar(gl_vendor);
	Import.cvars->RegisterVar(gl_renderer);
	Import.cvars->RegisterVar(gl_version);
	Import.cvars->RegisterVar(gl_extensions);
	Import.cvars->RegisterVar(gl_maxTextureSize);
	Import.cvars->RegisterVar(gl_maxTextureUnits);
	Import.cvars->RegisterVar(gl_dither);
	Import.cvars->RegisterVar(r_ext_compiledVertexArray);
	Import.cvars->RegisterVar(r_ext_multitexture);
	Import.cvars->RegisterVar(r_ext_swapInterval);
	Import.cvars->RegisterVar(r_vertexLight);
	Import.cvars->RegisterVar(r_clear);
	Import.cvars->RegisterVar(r_quality);
	Import.cvars->RegisterVar(r_grid);
	Import.cvars->RegisterVar(r_filter);
}


//
// Deletes an exported render
//
void __cdecl DeleteRender(IRender *pRender){
	delete (Render *) pRender;

	// FIXME: do that in dllmain
	// delete all the references to this module
	Import.cvars->UnregisterVar(gl_acceleration);
	Import.cvars->UnregisterVar(gl_vendor);
	Import.cvars->UnregisterVar(gl_renderer);
	Import.cvars->UnregisterVar(gl_version);
	Import.cvars->UnregisterVar(gl_extensions);
	Import.cvars->UnregisterVar(gl_maxTextureSize);
	Import.cvars->UnregisterVar(gl_maxTextureUnits);
	Import.cvars->UnregisterVar(gl_dither);
	Import.cvars->UnregisterVar(r_ext_compiledVertexArray);
	Import.cvars->UnregisterVar(r_ext_multitexture);
	Import.cvars->UnregisterVar(r_ext_swapInterval);
	Import.cvars->UnregisterVar(r_vertexLight);
	Import.cvars->UnregisterVar(r_clear);
	Import.cvars->UnregisterVar(r_quality);
	Import.cvars->UnregisterVar(r_grid);
	Import.cvars->UnregisterVar(r_filter);
}

