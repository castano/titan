/*===========================================================================
	Title: state.cpp
	Author: Ignacio Castaño Aguado
	Description: state implementation

===========================================================================*/
#include "render.h"




/*--------------------------------------------------------------------------
	Static declarations
--------------------------------------------------------------------------*/

Shader *		State::curr_shader=NULL;
int				State::curr_depthFunc=GL_LEQUAL;
int				State::curr_depthWrite=true;

int				State::curr_blenddst=GL_ONE;
int				State::curr_blendsrc=GL_ZERO;

int				State::curr_culling;
int				State::curr_alphaFunc;

int				State::skipping=0;

int				State::curr_texture[];


int				State::curr_lightmap=0;

int				State::pass=0;

int				State::last_tcgen[MAX_UNITS];



extern Var		r_quality;
extern Var		r_grid;
extern Var		gl_dither;


/*--------------------------------------------------------------------------
	Methods
--------------------------------------------------------------------------*/

void State::SetInitialState(int multitexture){

	// client state:
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	// texturing:
	glEnable(GL_TEXTURE_2D);
	curr_texture[0]=-1;
	
	// second texture unit
	if(multitexture){
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		//glEnable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_2D );
		//glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		curr_texture[1]=-1;
	}

	
	// backface culling:
	glEnable(GL_CULL_FACE);
//	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
//	glFrontFace(GL_CCW);		// ¿?
	curr_culling=CULL_FRONT;



	// color:
	glEnable(GL_COLOR);
	glShadeModel(GL_SMOOTH);
	glEnableClientState(GL_COLOR_ARRAY);


	if(gl_dither.ivalue) glEnable(GL_DITHER);
	else glDisable(GL_DITHER);


	// blending:
	glDisable(GL_BLEND);
	curr_blendsrc=GL_ONE;
	curr_blenddst=GL_ZERO;

	// alpha func
	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_ALWAYS, 1.0f );

	// depth test:
	glEnable(GL_DEPTH_TEST);	
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	curr_depthFunc=GL_LEQUAL;
	curr_depthWrite=true;


	glEnable(GL_SCISSOR_TEST);


	glEnable(GL_POLYGON_OFFSET_FILL);
	if(r_grid.ivalue) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glPolygonMode(GL_FRONT, GL_FILL);
	}



	switch(r_quality.ivalue){
		case 0:
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		break;
		case 1:
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_DONT_CARE);
		break;
		case 2:
		default:
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		break;
	}


	glReadBuffer( GL_BACK );
	glDrawBuffer( GL_BACK );


	// clearing:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
}



//
// not done
//
void State::SetDefaultState(void){
	pass=0;

}



//
// binds the texture of the given layer
//
// FIXME: it is incredible how much time does the engine spend here!
void State::BindTexture(Layer *l, int unit){
	int num;

	if(l->flags&LF_LIGHTMAP) {
		num=curr_lightmap;
	}
	else if(!l->map) return;	// FIXME: that happens???
	else if(l->num_maps>1){
		int frame=(appFloor(Import.timer->fTime*l->anim_speed))%l->num_maps;
		num=l->map[frame]->num;
	}
	else num = l->map[0]->num;

	if(State::curr_texture[unit]!=num){
		glBindTexture(GL_TEXTURE_2D, num);
		State::curr_texture[unit]=num;
	}
}



//
// set the appropiate state to render the layer
//
// FIXME: check all redundant state chantes ???
void State::setState(Layer *l){

	if(State::curr_blendsrc!=l->blendsrc || State::curr_blenddst!=l->blenddst){
		if(l->blendsrc==GL_ONE && l->blenddst==GL_ZERO) glDisable(GL_BLEND);
		else {
			if(State::curr_blendsrc==GL_ONE && State::curr_blenddst==GL_ZERO)
				glEnable(GL_BLEND);
			glBlendFunc(l->blendsrc, l->blenddst);
		}
		State::curr_blenddst=l->blenddst;
		State::curr_blendsrc=l->blendsrc;
	}
	if(State::curr_alphaFunc!=l->alphafunc){
		if(l->alphafunc==GL_ALWAYS) glDisable(GL_ALPHA_TEST);
		else {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(l->alphafunc, l->alphafuncref);
		}
		State::curr_alphaFunc=l->alphafunc;
	}
	
	if(/*State::pass ||*/ State::curr_shader->surface_flags&SF_POLYGONOFFSET) glPolygonOffset(1.0, 1.0);	// FIXME: Quake 3 uses (1,2); why?

	if(State::curr_depthFunc!=l->depthFunc){
		glDepthFunc(l->depthFunc);
		State::curr_depthFunc=l->depthFunc;
	}

	if(l->depthWrite!=State::curr_depthWrite){
		glDepthMask(l->depthWrite);
		State::curr_depthWrite=l->depthWrite;
	}
};


//
// evaluates a deform function
//
float State::Eval(float *p) {
    float x, y;

    // Evaluate a number of time based periodic functions
    // y = p[1] + p[2] * func( (time + p[3]) * p[4] )
    
    x = (Import.timer->fTime) * p[4] + p[3];
    x -= floor(x);		// normalized

    switch (appRound(p[0])) {
		case FUNC_SIN:
			y = FastSin(x * 2.0f * M_PI);
	    break;
		case FUNC_SQUARE:
			if (x < 0.5f) y = 1.0f;
			else y = -1.0f;
	    break;
		case FUNC_SAWTOOTH:
			y = x;
		break;
		case FUNC_INVERSESAWTOOTH:
			y = 1.0f - x;
		break;
		case FUNC_TRIANGLE:
			if (x < 0.5f) y = 4.0f * x-1.0f;
		    else y = -4.0f * x + 3.0f;
	    break;
		case FUNC_NOISE:
			// TODO: random noise
		break;
    }

    return y * p[2] + p[1];
}









