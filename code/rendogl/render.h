/*===========================================================================
	Title: Render.h
	Author: Ignacio Castaño Aguado
	Description: Render dll main header
	Classes:
		State		- Records the current state
===========================================================================*/
#ifndef _RENDER_H__
#define _RENDER_H__


/*--------------------------------------------------------------------------
	Definitions
--------------------------------------------------------------------------*/


#define MAX_STAGES		8		// more than 8 stages...
#define MAX_UNITS		2



/*--------------------------------------------------------------------------
	Headers
--------------------------------------------------------------------------*/

//
// Shared Files
//
#include "shared.h"


//
// Interfaces
//
#include "../interfaces/framework.h"
#include "../interfaces/render.h"


//
// Librarys
//

#ifdef WIN32
#include "../glsetup/gls.h"
#include "../glsetup/glext.h"
#include <float.h>	// to set low precision floating point
#endif


//
// Render Files
//
#include "driver.h"					// ogl driver loading and initialization



/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/


//
// Records the current state
//
class State {
public:

	// there is only a current state, thus all the members are static
	static void SetInitialState(int multitexture);
	static void SetDefaultState(void);


	static void BindTexture(Layer *l, int unit);
	static void setState(Layer *l);
	static float Eval(float *p);

	// input related
	static Shader *		curr_shader;
	static int			curr_depthFunc;
	static int			curr_depthWrite;
	static int			curr_blendsrc;
	static int			curr_blenddst;
	static int			curr_culling;
	static int			curr_alphaFunc;

	static int			curr_texture[4];	// currently the only card that supports 4 texturing units is the geforce

	static int			skipping;
	
	static int			curr_lightmap;

	static int			last_tcgen[MAX_UNITS];


	// output related
	static int		pass;
};


/*---------------------------------------------------------------------------
	Functions
---------------------------------------------------------------------------*/

//
// GL Utils:
//

void ScaleImage(GLenum format, INT inwidth, INT inheight, const BYTE *in, INT outwidth, INT outheight, BYTE *out);
void BuildMipmaps(GLenum target, INT width, INT height, GLenum format, const BYTE *data);

GLint __gluBuild2DMipMaps(GLint component, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);


// taken from mesa:			(TODO: use our custom functions)
GLint glScaleImage( GLenum format, 
					GLint widthin, GLint heightin,
					GLenum typein, const void *datain,
					GLint widthout, GLint heightout,
					GLenum typeout, void *dataout );


GLint glBuild2DMipmaps (GLenum target, GLint component, 
						GLsizei width, GLsizei height, 
						GLenum format, GLenum type, 
						const void *data);






#endif	//_RENDER_H__