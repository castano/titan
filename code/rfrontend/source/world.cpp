/*===========================================================================
	Title: world.h
	Author: Ignacio Castaño Aguado
	Description: World database and services
	Classes:
		World	- database for the render, sound and entities, provides rendering
				  sound playing, and colision detection.

===========================================================================*/
#include "rfrontend.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IRender *		render;	// render
extern Error *			err;
extern IOutputDevice *	out;
extern IAlloc *			GMalloc;


/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

World::World(void){
		
	current_cam=NULL;

};


// add an object to the world
void World::Add(Object *obj) {

	scene.Add(obj);

};


bool World::AddMap(const char *name){
	Q3BSP *bsp=NULL;
	char mappath[1024]=MAP_PATH;

	strcat(mappath, name);

	// replace extension
	f_StripExtension(mappath);
	strcat(mappath, ".bsp");

	bsp = new Q3BSP(mappath);

	if(err->GetLastError()) return false;

	Add(bsp);

	return true;
};


// FIXME: heap gets corrupted here:
void World::Init(void){
	// load shader that have been referenced, and load also his textures
	// (shaders must be loaded before any other object)
	shaders.Update();

	// textures must be loaded after shaders
	texmng.Update();


	// TODO: every object should get the current pointer to his shaders in the
	//		 initialization instead of calling GetShader every frame.
	scene.Init();
	over.Init();
}

void World::Shut(void){
	shaders.ResetAll();
	shaders.Update();
	out->Insert("--- Freeing Textures\n");
	texmng.ResetAll();
	texmng.Update();

	scene.DeleteAll();
}


void World::Render(Camera *cam){
	current_cam=cam;


	frustum.setup(cam);								// set up frustum planes

	scene.Update();									// update the scene (moving objects, visibility determination)

	render->BeginFrame();								// clears the screen and the z buffer

		render->SetViewPort((camera_s *) cam, 1);		// set up modelview matrix
			
			scene.Render();								// render the scene

		render->SetViewPort((camera_s *) cam, 0);		// set up 2d viewport

			over.Render();								// render overalay

	render->EndFrame();									// flips the screen
}
