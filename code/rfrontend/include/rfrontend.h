/*===========================================================================
	Title: rfrontend.h
	Author: Ignacio Castaño Aguado
	Description: main header file of the render fronend module

===========================================================================*/
#ifndef _RENDER_FRONTEND__
#define _RENDER_FRONTEND__


#include "shared.h"
#include "../../interfaces/render.h"





/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define MAP_PATH "maps/"

//
// Frustum planes:
//
enum {
	PL_LEFT,
	PL_RIGHT,
	PL_BOTTOM,
	PL_TOP,
	PL_NEAR,
	PL_FAR,
	NUM_PLANES
};

enum {
	PLB_NONE=0x00,
	PLB_LEFT=0x01,
	PLB_RIGHT=0x02,
	PLB_BOTTOM=0x04,
	PLB_TOP=0x08,
	PLB_NEAR=0x10,
	PLB_FAR=0x20,
	PLB_ALL=PLB_LEFT|PLB_RIGHT|PLB_BOTTOM|PLB_TOP,
};

// plane types are used to speed some tests
// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2
#define	PLANE_NON_AXIAL	3


//
// Face types
//
enum {
    FACETYPE_NORMAL	= 1,
	FACETYPE_PLANAR	= 1,		// the same as above
    FACETYPE_PATCH	= 2,
    FACETYPE_MESH	= 3,
    FACETYPE_FLARE	= 4
};


/*---------------------------------------------------------------------------
	Structures:
---------------------------------------------------------------------------*/

//
// Planes
//
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	BYTE	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	BYTE	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	BYTE	pad[2];			// 2 bytes for padding puspouses
} cplane_t;

typedef struct fixplane_s {
	int 	normal[3];
	int		dist;
	int		indexes[6];
	//	FIXME: add padding for mmx opts?
} fixplane_t;

struct frustum_s {
	cplane_t planes[NUM_PLANES];
	fixplane_t fixplanes[NUM_PLANES];
	int	num_planes;
};



//
// Faces (or surfaces)
//
typedef struct {
    int shader;			// Shader reference
    int unknown1;
    int facetype;		// FACETYPE enum
    int firstvert, numverts;
    int firstelem, numelems;
    int lm_texnum;		// lightmap info
    int lm_offset[2];
    int lm_size[2];
    vec3_t v_orig;		// FACETYPE_NORMAL only
    bboxf_t bbox;		// FACETYPE_MESH only
    vec3_t v_norm;		// FACETYPE_NORMAL only
    int mesh_cp[2];		// mesh control point dimensions
} face_t;


class PlanarFace : public Surface {
};

class Mesh : public Surface {
};

class Patch : public Surface {
};


/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

class Frustum {
private:
	cplane_t planes[NUM_PLANES];
	fixplane_t fixplanes[NUM_PLANES];
	int	num_planes;		// FIXME: instead of num use a mask
	vec3_t center;		// center of coordinates 
public:

	void setup(camera_s *cam);					// build the frustum from the camera
	//void setup(vec3_t point, polygon_s *poly);	// build the frustum for a polygon
	//void setup(vec3_t point, portal_s *portal);	// build the frustum for a portal
	void setup(vec3_t point, bbox_t box);		// build the frustum for a box
	void setup(vec3_t point, vec3_t quad);		// build the frustum for given quad
	void updateClipPlanes(void);

	int testPoint(vec3_t point, int &planeflags);
	int testSphere(vec4_t sphere, int &planeflags);
	int testBBox(bbox_t box, int &planeflags);
	int testBBox(bboxf_t box);
	int testFastBox(bbox_t box, int &planeflags);

};




/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

#include "TShaderParser.h"
#include "object.h"
#include "shader.h"
#include "layer.h"
#include "BezierPatch.h"
#include "bsp.h"
#include "overlay.h"
#include "camera.h"
#include "shaderfile.h"
#include "tex.h"
#include "scene.h"
#include "world.h"







#endif // _RENDER_FRONTEND__