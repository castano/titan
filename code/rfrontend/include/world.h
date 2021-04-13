/*===========================================================================
	Title: world.h
	Author: Ignacio Castaño Aguado
	Description: World database and services
	Classes:
		World	- database for the render, sound and entities, provides rendering
				  sound playing, and colision detection.

===========================================================================*/
#ifndef _WORLD_H_
#define _WORLD_H_







/*---------------------------------------------------------------------------
	World:
---------------------------------------------------------------------------*/


class World {
public:
	World(void);


	void Add(Object *obj);
	bool AddMap(const char *name);

	void Init(void);
	void Shut(void);
	void Update(void);
	void Render(Camera *cam);

public:

	// TODO: use a stack of frustums and cameras
	Camera *	current_cam;	// FIXME: replace with viewport
	Frustum		frustum;


	// objects of the world:
	Overlay		over;			// overlay
	ShaderMng	shaders;		// FIXME: fixed number of shaders???
	LayerMng	layers;
	TexMng		texmng;

	// benchmarks:
	DWORD		find_leaf;
	DWORD		mark_nodes;
	DWORD		mark_faces;
	DWORD		push_faces;

private:
	

	Scene		scene;

};






#endif //_WORLD_H_