/*===========================================================================
	Title: layer.cpp
	Author: Ignacio Castaño Aguado
	Description: hmmm see layer.h
	Classes:
		LayerMng				- layer manager

===========================================================================*/
#include "rfrontend.h"





/*---------------------------------------------------------------------------
	Globals
---------------------------------------------------------------------------*/

extern IOutputDevice *	out;
extern IRender *		render;
extern Error *			err;
extern IAlloc *			GMalloc;




/*---------------------------------------------------------------------------
	LayerMng
---------------------------------------------------------------------------*/


LayerMng::LayerMng(void){
	guard(LayerMng::LayerMng);
	unguard;
}

LayerMng::~LayerMng(void){
	guard(LayerMng::~LayerMng);
	unguard;
}


//
// CreateLayer allocates a layer and fills it with default values
//
Layer *LayerMng::CreateLayer(void){
	guard(LayerMng::CreateLayer);

	Layer *l= new Layer;

	l->flags=0;
	l->num_maps=0;
	l->anim_speed=0;
	
	for(int i=0; i<MAX_ANIMFRAMES; i++) l->map[i]=NULL;

	l->tcMod=NULL;
	l->aGen=NULL;

	l->tcGen=TCGEN_BASE;
	l->rgbGen=RGBGEN_IDENTITY_LIGHTING;
	//l->rgbGen=RGBGEN_IDENTITY;

	l->alphafunc=IntForConst("GL_ALWAYS");
	l->alphafuncref=1.0f;

	l->blendsrc=IntForConst("GL_ONE");
	l->blenddst=IntForConst("GL_ZERO");

	l->depthFunc=IntForConst("GL_LEQUAL");
	l->depthWrite=true;

	return l;
	unguard;
}

void LayerMng::DeleteLayer(Layer *l){

	if(l!=NULL){
		funclist *f, *fnext;
		for(f=l->tcMod; f; f=fnext){
			fnext=f->next;
			delete f;
		}
		l->tcMod=NULL;
		for(f=l->aGen; f; f=fnext){
			fnext=f->next;
			delete f;
		}
		l->aGen=NULL;

		// NOTE: we do not delete the map here, because it is shared by other layers.
	}

	delete l;
}


/*---------------------------------------------------------------------------
	End
---------------------------------------------------------------------------*/
