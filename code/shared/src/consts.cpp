/*===========================================================================
	Title: consts.cpp
	Author: Ignacio Castaño Aguado
	Description: constants lookup
	TODO: 
		- add more constants
		- add not ogl-related constants 

===========================================================================*/
#include "shared.h"


#define NUM_CONSTANTS	52


/*-----------------------------------------------------------------------------
	Types:
-----------------------------------------------------------------------------*/

struct constants{
	char *cad;
	int value;
};




/*-----------------------------------------------------------------------------
	Constants:
-----------------------------------------------------------------------------*/

const struct constants clist[128]={
"FALSE",					0,
"GL_ALWAYS",				0x0207,
"GL_BACK",					0x0405,
"GL_CLAMP",					0x2900,
"GL_DECAL",					0x2101,
"GL_DECR",					0x1E03,
"GL_DST_ALPHA",				0x0304,
"GL_DST_COLOR",				0x0306,
"GL_EQUAL",					0x0202,
"GL_EYE_LINEAR",			0x2400,
"GL_EYE_PLANE",				0x2502,
"GL_FALSE",					0,
"GL_FLAT",					0x1D00,
"GL_FRONT",					0x0404,
"GL_FRONT_AND_BACK",		0x0408,
"GL_GEQUAL",				0x0206,
"GL_GREATER",				0x0204,
"GL_INCR",					0x1E02,
"GL_KEEP",					0x1E00,
"GL_LEQUAL",				0x0203,
"GL_LESS",					0x0201,
"GL_LINEAR",				0x2601,
"GL_LINEAR_MIPMAP_LINEAR",	0x2703,
"GL_LINEAR_MIPMAP_NEAREST",	0x2701,
"GL_MODULATE",				0x2100,
"GL_NOTEQUAL",				0x0205,
"GL_NEAREST",				0x2600,
"GL_NEAREST_MIPMAP_LINEAR",	0x2702,
"GL_NEAREST_MIPMAP_NEAREST",0x2700,
"GL_NEVER",					0x0200,
"GL_OBJECT_LINEAR",			0x2401,
"GL_OBJECT_PLANE",			0x2501,
"GL_ONE",					1,
"GL_ONE_MINUS_DST_ALPHA",	0x0305,
"GL_ONE_MINUS_DST_COLOR",	0x0307,
"GL_ONE_MINUS_SRC_ALPHA",	0x0303,
"GL_ONE_MINUS_SRC_COLOR",	0x0301,
"GL_REPEAT",				0x2901,
"GL_REPLACE",				0x1E01,
"GL_SPHERE_MAP",			0x2402,
"GL_SMOOTH",				0x1D01,
"GL_SRC_ALPHA",				0x0302,
"GL_SRC_ALPHA_SATURATE",	0x0308,
"GL_SRC_COLOR",				0x0300,
"GL_TEXTURE_ENV",			0x2300,
"GL_TEXTURE_ENV_COLOR",		0x2201,
"GL_TEXTURE_ENV_MODE",		0x2200,
"GL_TEXTURE_GEN_MODE",		0x2500,
"GL_TEXTURE_MAG_FILTER",	0x2800,
"GL_TEXTURE_MIN_FILTER",	0x2801,
"GL_TEXTURE_WRAP_S",		0x2802,
"GL_TEXTURE_WRAP_T",		0x2803,
"GL_TRUE",					1,
"GL_ZERO",					0,	
"TRUE",						1,
"ZZZZ",						0,
"ZZZZ",						0,
};



/*-----------------------------------------------------------------------------
	Look up:
-----------------------------------------------------------------------------*/

int IntForConst(const char *name){
	int prin=0;
	int fin=NUM_CONSTANTS;
	int medio;
	int dir;

	while(prin<=fin){
		medio=(prin+fin)>>1;
		
		dir = stricmp(clist[medio].cad, name);
		if(dir<0) prin=medio+1;
		else if(dir>0) fin=medio-1;
		else return clist[medio].value;
	}
	return 0;
}
