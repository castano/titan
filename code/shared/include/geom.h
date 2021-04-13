/*============================================================================
	Title: geom.h
	Author: Ignacio Castaño Aguado
	Description: shared definitions related to the geometry

============================================================================*/
#ifndef _GEOM_H_
#define _GEOM_H_


/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

#include "consts.h"




/*---------------------------------------------------------------------------
	Definitions
---------------------------------------------------------------------------*/

#define MAX_LAYERS				8
#define MAX_ANIMFRAMES			8
#define MAX_TCMOD				8
#define MAX_IMAGES				2048

#define MAX_PARAMS				8


// Transform functions
enum FUNC{
    FUNC_CONST,
	FUNC_SIN,
    FUNC_TRIANGLE,
    FUNC_SQUARE,
    FUNC_SAWTOOTH,
    FUNC_INVERSESAWTOOTH,
	FUNC_NOISE
};

// functions for generating and deforming vertexes.
enum RGBGEN{
	RGBGEN_IDENTITY_LIGHTING,	// 0
	RGBGEN_IDENTITY,			// 0
	RGBGEN_WAVE,				// 5
	RGBGEN_ENTITY,				// 0
	RGBGEN_ONE_MINUS_ENTITY,	// 0
	RGBGEN_VERTEX,				// 0
	RGBGEN_ONE_MINUS_VERTEX,	// 0
	RGBGEN_LIGHTING_DIFFUSE,	// 0
};

// alpha channel generation
enum ALPHAGEN{
	ALPHAGEN_PORTAL,			// 0
};

// texture coordinate generation
enum TCGEN{
	TCGEN_BASE,					// 0
	TCGEN_ENVIRONMENT,			// 0
	TCGEN_LIGHTMAP,				// 0
	TCGEN_VECTOR,				// 6
};

// texture coordinate deformation
enum TCMOD{
	TCMOD_ROTATE,				// 1
	TCMOD_SCALE,				// 2
	TCMOD_SCROLL,				// 2
	TCMOD_STRETCH,				// 5
	TCMOD_TRANSFORM,			// 7
	TCMOD_TURB,					// 4
};

// vertes coordinates deformations
enum DEFORMVERTEXES{
	DEFORMVERTEXES_NONE,		// 0
	DEFORMVERTEXES_WAVE,		// 6
	DEFORMVERTEXES_NORMAL,		// 5
	DEFORMVERTEXES_BULGE,		// 3
	DEFORMVERTEXES_MOVE,		// 8
	DEFORMVERTEXES_AUTOSPRITE,	// 0
	DEFORMVERTEXES_AUTOSPRITE2,	// 0
};

// layer flags 
enum {
	LF_NOMIPMAP=0x01,		// do not create mipmaps
	LF_NOPICMIP=0x02,		// allways pick the highest mipmap
	LF_CLAMP=0x04,			// clamp, default is repeat
	LF_DETAIL=0x08,			// detail texture
	LF_LIGHTMAP=0x10,		// lightmap
	LF_HASTCMOD=0x20,
	LF_USENORMALS=0x40,
	LF_MULTITEX=0x80		// this shader and the next could be collapsed
};


enum {
	SORT_PORTAL=1,
	SORT_SKY=2,
	SORT_NORMAL=3, SORT_OPAQUE=3,
	SORT_DECAL=4,
	SORT_UNDERBANNER=5,	
	SORT_BANNER=6,
	SORT_TRANS=7, SORT_SEETHROUGH=7,
	SORT_UNDERWATER=8,
	SORT_ADDITIVE=9,
	SORT_ADDITIVE1=10,			// ¿?
	SORT_ADDITIVE2=11,
	SORT_FOG=12,
	SORT_FOG1=13,				// ¿?
	SORT_FOG2=14,				// ¿?
	SORT_STENCILSHADOW=15,		// ¿? taken from shaderlib
	SORT_NEAREST=16,
};

// culling type:
enum {
	CULL_FRONT=1,
	CULL_BACK=-1,
	CULL_NONE=0,
};

// Shader flags:
enum {
	SF_NOMIPMAP=0x01,			// do not create mipmaps
	SF_NOPICMIP=0x02,			// allways pick the highest mipmap
	SF_POLYGONOFFSET=0x04,		// surface needs polygon offset
	SF_PORTAL=0x08,				// there is a portal layer
	SF_HASLIGHTMAP=0x10,		// there is a lightmap layer
	SF_HASCOLORS=0x20,			// there is a layer that needs colors
	SF_HASTCMOD=0x40,			// there is a layer that has lf_hastcmod
	SF_USENORMALS=0x80,			// there is a layer that has lf_usenormals
	SF_MULTITEX=0x0100			// this shader has two layers that can be collapsed
};





/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

// tex info
typedef struct {
	unsigned int num;
	byte *mem;
	int width;
	int height;
	int bpp;
}texinfo;


// list of functions for sucesive vertex or coordinates modification.
typedef struct _funclist {
	int func;
	int parameters;
	float p[MAX_PARAMS];
	_funclist *next;
}funclist;




/*---------------------------------------------------------------------------
	Geometry:
---------------------------------------------------------------------------*/


// Interleaved vertex info
typedef struct {
    vec3_t v_point;     // World coords
    texcoord_t tex_st;  // Texture coords
    texcoord_t lm_st;   // Lightmap texture coords
    vec3_t v_norm;      // Normal
    colour_t colour;    // Colour used for vertex lighting
} vertex_t;


/*
the frontend can distinguish between surfaces, but the backend not.
Surface has only the variables that all the Surfaces share, and the info, that 
the backend needs, i.e. the concept of dist only exists for the planar faces, but it 
is needed by the shader command alphaGen, or v_orig, that is needed by deformVertexes
autosprite
*/
class Surface {
public:
	int			framenum;
	
	int			facetype;		// FACETYPE enum

	int			shader;			// Shader reference
	int			lm_texnum;		// lightmap info

	vertex_t *	firstvert;		// TODO: should be 'void *' + 'int vertex_id'
	int			numverts;
	int *		firstelem;
	int			numelems;

	int			dist;
	vec3_t		v_norm;
	vec3_t		v_orig;

	bboxf_t		bbox;
};






/*---------------------------------------------------------------------------
	State:
---------------------------------------------------------------------------*/

//
// Layer Class: (describes each pass)
//
typedef struct {

	int flags;						// layer flags

	float anim_speed;				// speed of the animation
	int num_maps;					// number of texures (for animated layers)
	texinfo *map[MAX_ANIMFRAMES];	// array of the maps

	funclist *tcMod;		// texture coordinates modifiers
	funclist *aGen;			// alpha color generation

	TCGEN	tcGen;

	RGBGEN	rgbGen;
	float	rgbGenParams[5];


	int		alphafunc;		// alpha function
	float	alphafuncref;

	int		blendsrc;		// blend function
	int		blenddst;

	int		depthFunc;		// depth func
	int		depthWrite;		// touch depth buffer
}Layer;


//
// Shader: describes how a surface is rendered
//
typedef struct {

	int		surface_flags;		// surface_flags (described in surface_flags, come from the bsp, 
								// and not from the shader definition)
	// contents of the shader ?
	int		contents;

	// culling type
	int		cull;

	int		sortvalue;			// Carmack changed int sortvalues to float, why?

	// flags to determine misc properties of the shader, like polygonoffset, multitexture
	// mipmap, nopicmip, haslightmap, portal... and to determine wich info of the datasource
	// we need to render the surface
	int		flags;

	// layers or stages
	int		num_layers;			
	Layer *	layer[MAX_LAYERS];

	// vertex deformation			
	// FIXME: we allow only one vertex deformation, but q3 documentation says that more than
	//		  one are allowed.
	DEFORMVERTEXES		deformVertexes;
	float				deformVertexes_params[8];

}Shader;





/*---------------------------------------------------------------------------
	Camera:
---------------------------------------------------------------------------*/


//
// Camera class
//
// there is a lot of redundant information, but that can save us some calcs.
class camera_s {
	// this functions only are implemented in the camera class
	// they are here only for padding purpouses
	virtual void Init(float fov, float w, float h, float x, float y)=0;
public:
	float c[3];				// position
	float yaw;				// left / right
	float pitch;			// up / down
	float roll;				// fall over

	float fov;				// fov in radians
	float cos_fov;			// cos(fov)
	float proj_ratio;		// 4/3 or 16/9

	float Rotation[9];		// precalculated 3x3 rotation matrix for applying movements

	float eyedir[3];		// eyedir, and normal of the viewing z=0 plane
	float eyedist;			// signed distance to the z=0 plane

	int sx, sy, width, height;
};



#endif // _GEOM_H_


