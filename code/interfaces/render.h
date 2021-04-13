/*===========================================================================
	Title: framework.h
	Author: Ignacio Castaño Aguado
	Description: interfaces of the render module

===========================================================================*/
#ifndef _RENDER_INTERFACE_
#define _RENDER_INTERFACE_



/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

// geometry definitions that are shared beetween the render and the app
#include "geom.h"

#define MAX_DRIVERS		4
#define NAME_LENGTH		128

typedef struct {
	int current_driver;
	int num_drivers;
	char driver_names[MAX_DRIVERS][NAME_LENGTH];
}render_info;

typedef struct {
	int show_triangles;			// true / false
	int mode;					// 0=dont draw, 1=textured(default), 2=gouraud, 3=overdraw
	int use_extensions;			// true / false
	int use_ogllighting;
	int clearscreen;
}render_state;



/*---------------------------------------------------------------------------
	Interfaces:
---------------------------------------------------------------------------*/


class IRender {
public:
	virtual int	Init(WindowHandle )=0;
	virtual void Shut()=0;

	virtual int RegisterTexInfo(texinfo *, int flags)=0;
	virtual void UnregisterTexInfo(texinfo *tex)=0;

	// begin/end:
	virtual int	BeginFrame()=0;
	virtual int	EndFrame()=0;

	virtual void SetViewPort(camera_s *, int type)=0;
	virtual void SetRenderState(Shader *, int lightmap)=0;
	//virtual void SetDataSource(DataSource *)=0;
	//virtual void PushTriangles(int *first_elem, int numelems, int first_vertex, int num_verts)=0;
	//virtual void PushTriangles(int *first_elem, int numelems, vertex_t *first_vertex, int num_verts)=0;
	virtual void PushTriangles(Surface *s)=0;


	// misc:
	virtual void Screenshot(byte *buffer, int left, int top, int right, int bottom)=0;
	virtual void Lock(void)=0;
	virtual void Unlock(void)=0;

	// state/caps:
	virtual void QueryInfo(render_info *)=0;
	virtual void SetState(render_state *)=0;
};


















#endif _RENDER_INTERFACE_