/*===========================================================================
	Title: overlay.cpp
	Author: Ignacio Castaño Aguado
	Description: draws bitmaps and strings
	Classes:
		overlay					- overlay extends Object
===========================================================================*/
#include "rfrontend.h"	// main header of the frontend files




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IRender *render;
extern World *world;




/*---------------------------------------------------------------------------
	Private Data:
---------------------------------------------------------------------------*/

// rgba in little endian is abgr
#ifdef BIGENDIAN
static const int colors[16]={
	~0,								// white	0
	(255<<24)|(0  <<16)|(0  <<8)|255,	// red		1
	(0  <<24)|(255<<16)|(0  <<8)|255,	// green	2
	(0  <<24)|(0  <<16)|(255<<8)|255,	// blue		3
	(255<<24)|(255<<16)|(0  <<8)|255,	// magenta	4
	(255<<24)|(0  <<16)|(255<<8)|255,	// yellow	5
	(0  <<24)|(255<<16)|(255<<8)|255,	// cyan		6
	255,							// black	7
};
#else
// colors are divided by 2 if we have overbright bits !!
static const int colors[16]={
	(255<<24)|(127<<16)|(127<<8)|127,	// white	0
	(255<<24)|(0  <<16)|(0  <<8)|127,	// red		1
	(255<<24)|(0  <<16)|(127<<8)|0,		// green	2
	(255<<24)|(127<<16)|(0  <<8)|0,		// blue		3
	(255<<24)|(127<<16)|(0  <<8)|127,	// magenta	4
	(255<<24)|(0  <<16)|(127<<8)|127,	// yellow	5
	(255<<24)|(127<<16)|(127<<8)|0,		// cyan		6
	(255<<24),							// black	7
};
#endif




/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

void Overlay::Init(void){
	num_quads=0;
	num_elems=0;
	num_verts=0;

	// write a default constructor for the surface, instead of clearing it
	appMemzero(&quads, sizeof(Surface)*MAX_QUADS);
}


void Overlay::Render(void){
	int last_shader=-1;

	for(int i=0; i<num_quads; i++){
		if(last_shader!=quads[i].shader){
			last_shader=quads[i].shader;
			Shader *s=world->shaders.GetShader(quads[i].shader);
			render->SetRenderState(s, 0);
		}
		render->PushTriangles(&quads[i]);
	}

	num_quads=0;
	num_elems=0;
	num_verts=0;
}


void Overlay::Quad(int shader, float x, float y, float w, float h, float repx, float repy){
	
	quads[num_quads].firstelem=elems;
	quads[num_quads].numelems=6;
	quads[num_quads].firstvert=verts+num_verts;
	quads[num_quads].numverts=4;
	quads[num_quads].shader=shader;
	
	elems[num_elems+0]=num_verts+0;
	elems[num_elems+1]=num_verts+1;
	elems[num_elems+2]=num_verts+2;
	elems[num_elems+3]=num_verts+2;
	elems[num_elems+4]=num_verts+1;
	elems[num_elems+5]=num_verts+3;

	verts[num_verts+0].tex_st[0]=0.0f;
	verts[num_verts+0].tex_st[1]=0.0f;
	verts[num_verts+0].colour=~0;
	verts[num_verts+0].v_point[0]=x;
	verts[num_verts+0].v_point[1]=y;
	verts[num_verts+0].v_point[2]=0;
	
	verts[num_verts+1].tex_st[0]=repx;
	verts[num_verts+1].tex_st[1]=0.0f;
	verts[num_verts+1].colour=~0;
	verts[num_verts+1].v_point[0]=x+w;
	verts[num_verts+1].v_point[1]=y;
	verts[num_verts+1].v_point[2]=0;

	verts[num_verts+2].tex_st[0]=0.0f;
	verts[num_verts+2].tex_st[1]=repy;
	verts[num_verts+2].colour=~0;
	verts[num_verts+2].v_point[0]=x;
	verts[num_verts+2].v_point[1]=y+h;
	verts[num_verts+2].v_point[2]=0;

	verts[num_verts+3].tex_st[0]=repx;
	verts[num_verts+3].tex_st[1]=repy;
	verts[num_verts+3].colour=~0;
	verts[num_verts+3].v_point[0]=x+w;
	verts[num_verts+3].v_point[1]=y+h;
	verts[num_verts+3].v_point[2]=0;

	num_quads+=1;
	num_elems+=6;
	num_verts+=4;
}


void Overlay::String(char *str, int shader, float x, float y, float size, int color){
	float xnext;
	float xpos, ypos;

	// TODO: test elems and verts boundaries !!
	if(num_quads>=MAX_QUADS){
		OutputDebugString("Buffer overflow!!!");
		return;
	}
	
	int num_chars=strlen(str);
	
	quads[num_quads].firstelem=elems;
	quads[num_quads].firstvert=verts+num_verts;
	quads[num_quads].shader=shader;

	quads[num_quads].numelems=num_chars*6;
	quads[num_quads].numverts=num_chars*4;


	for(int i=0; i<num_chars; i++){

		xpos=(float)((str[i]&15)<<4)/256.0f;
		ypos=(float)((str[i]>>4)<<4)/256.0f;
		xnext=x+8*size;

		if(str[i]==' ') {
			x=xnext;
			quads[num_quads].numelems-=6;
			quads[num_quads].numverts-=4;
			continue;
		}

	//	if(num_elems>=MAX_ELEMS-6) return;
	//	if(num_verts>=MAX_VERTS-4) return;

		elems[num_elems+0]=num_verts+0;
		elems[num_elems+1]=num_verts+1;
		elems[num_elems+2]=num_verts+2;
		elems[num_elems+3]=num_verts+2;
		elems[num_elems+4]=num_verts+1;
		elems[num_elems+5]=num_verts+3;

		verts[num_verts+0].tex_st[0]=xpos;
		verts[num_verts+0].tex_st[1]=ypos;
		verts[num_verts+0].colour=colors[color];
		verts[num_verts+0].v_point[0]=x;
		verts[num_verts+0].v_point[1]=y;
		verts[num_verts+0].v_point[2]=0;
	
		verts[num_verts+1].tex_st[0]=xpos+15.0f/256.0f;
		verts[num_verts+1].tex_st[1]=ypos;
		verts[num_verts+1].colour=colors[color];
		verts[num_verts+1].v_point[0]=xnext;
		verts[num_verts+1].v_point[1]=y;
		verts[num_verts+1].v_point[2]=0;

		verts[num_verts+2].tex_st[0]=xpos;
		verts[num_verts+2].tex_st[1]=ypos+16.0f/256.0f;
		verts[num_verts+2].colour=colors[color];
		verts[num_verts+2].v_point[0]=x;
		verts[num_verts+2].v_point[1]=y+16*size;
		verts[num_verts+2].v_point[2]=0;

		verts[num_verts+3].tex_st[0]=xpos+15.0f/256.0f;
		verts[num_verts+3].tex_st[1]=ypos+16.0f/256.0f;
		verts[num_verts+3].colour=colors[color];
		verts[num_verts+3].v_point[0]=xnext;
		verts[num_verts+3].v_point[1]=y+16*size;
		verts[num_verts+3].v_point[2]=0;
		
		x=xnext;
		num_elems+=6;
		num_verts+=4;
	}
	
	num_quads+=1;
}

