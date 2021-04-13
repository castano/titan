/*===========================================================================
	Title: scene.cpp
	Author: Ignacio Castaño Aguado
	Description: List of objects
	Classes:
		Scene	- object list

===========================================================================*/
#include "rfrontend.h"



/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

Scene::Scene(void){
	first=NULL;
	current=first;
};


//
// initializes all the objects of the scene
//
void Scene::Init(void){
	for(linked_obj *l=first; l; l=l->next){
		l->obj->Init();
	}
}


//
// add an object to the scene
//
void Scene::Add(Object *obj) {
	linked_obj *lo= new linked_obj;
	lo->obj=obj;
	lo->next=first;
	first=lo;

	// reset current:
	current=first;
};


//
// Updates all the objects of the scene
//
void Scene::Update(void){
	for(linked_obj *l=first; l; l=l->next){
		l->obj->Update();
	}
}


//
// Renders all the objects of the scene
//
void Scene::Render(void){
	for(linked_obj *l=first; l; l=l->next){
		l->obj->Render();
	}
}



void Scene::DeleteAll(void){
	linked_obj *l, *lnext;
	for(l=first; l; l=lnext){
		lnext=l->next;
		switch(l->obj->Id()){
			case OB_Q3BSP:
				delete (Q3BSP*) l->obj;
			break;
			case OB_NONE:
			default:
				delete l->obj;
			break;
		}
		
	}
}