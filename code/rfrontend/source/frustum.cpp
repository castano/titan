/*===========================================================================
	Title: frustum.cpp
	Author: Ignacio Castaño Aguado
	Description: Frustum culling
	TODO: 
		- use a bbox of the frustum for fast rejection ???
		- use a custom struct for plane infos (not fixplanes and cplane) I dont
		  know if I really want to do that.
		- I have some great expectations for this class, but currently it is 
		  very simple.

	FIXME: 
		- i have proven that 4 planes are sometimes worse than 5 (but q1 uses 4)
===========================================================================*/
#include "rfrontend.h"





/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

//
// setup frustum planes for the camera
//
void Frustum::setup(camera_s *cam){
	vec3_t v;
	float c, s;
	num_planes=4;	// no near and far plane, 


	// near plane:
//	v[0]=0; v[1]=0; v[2]=-1.0;
//	FMatrix_MulFVec(cam->Rotation, v, planes[PL_NEAR].normal);
//	planes[PL_NEAR].dist= DotProduct(planes[PL_NEAR].normal, cam->c) + 10;


	// calc components of plane normals, (are constant if matrixview does not change)
	c=(float)FastCos(cam->fov/2.0f);
	s=(float)FastSin(cam->fov/2.0f);

	// Left plane
	v[0]=c; v[1]=0; v[2]=-s;
	MultVect3x3(cam->Rotation, v, planes[PL_LEFT].normal);
	planes[PL_LEFT].dist=DotProduct( planes[PL_LEFT].normal, cam->c);

	// Right plane
	v[0]=-c; v[1]=0; v[2]=-s;
	MultVect3x3(cam->Rotation, v, planes[PL_RIGHT].normal);
	planes[PL_RIGHT].dist=DotProduct( planes[PL_RIGHT].normal, cam->c);


	// calc components of plane normals
	float fovy=atan(tan(cam->fov/2.0f)/cam->proj_ratio);
	c=(float)FastCos(fovy);
	s=(float)FastSin(fovy);

	// Bottom plane
	v[0]=0; v[1]=c; v[2]=-s;
	MultVect3x3(cam->Rotation, v, planes[PL_BOTTOM].normal);
	planes[PL_BOTTOM].dist=DotProduct(planes[PL_BOTTOM].normal,cam->c);

	// Top plane
	v[0]=0; v[1]=-c; v[2]=-s;
	MultVect3x3(cam->Rotation, v, planes[PL_TOP].normal);
	planes[PL_TOP].dist=DotProduct(planes[PL_TOP].normal,cam->c);

	
	// fixed point planes, for mmx optimizations...
	for(int i=0; i<num_planes; i++) {
		fixplanes[i].normal[0] = appRound( planes[i].normal[0] * 65536.0f );	//conversiones float-fix
		fixplanes[i].normal[1] = appRound( planes[i].normal[1] * 65536.0f );
		fixplanes[i].normal[2] = appRound( planes[i].normal[2] * 65536.0f );
		fixplanes[i].dist = appRound( planes[i].dist * 65536.0f );
	}


	updateClipPlanes();

}


//
// update clip planes
//
void Frustum::updateClipPlanes(void){
	cplane_t	*cplane;
	int			bits;

	for(int i=0; i<num_planes; i++){
		cplane=&planes[i];

		// this info is not used in frustum...
		if(cplane->normal[0]==1.0f) cplane->type=PLANE_X;
		else if(cplane->normal[1]==1.0f) cplane->type=PLANE_Y;
		else if(cplane->normal[2]==1.0f) cplane->type=PLANE_Z;
		else cplane->type=PLANE_NON_AXIAL;

		// for fast box on planeside test
		bits = 0;
		for (int j=0 ; j<3 ; j++) {
			if (cplane->normal[j] < 0) {
				bits |= 1<<j;
				fixplanes[i].indexes[j]=j;
				fixplanes[i].indexes[j+3]=j+3;
			}
			else {
				fixplanes[i].indexes[j]=j+3;
				fixplanes[i].indexes[j+3]=j;
			}
		}
		cplane->signbits = bits;
	}
}


//
// will test points against frustum planes
//
int Frustum::testPoint(vec3_t point, int &planeflags){
	return 0;
}


//
// will test spheres against frustum planes
//
int Frustum::testSphere(vec4_t sphere, int &planeflags){
	return 0;
}


//
//	test bounding box:
//		- returns 0 if completely ouside the frustum, 1 if inside or partially inside.
//		- planeflags are togled for the sub-boxes
//		- a box is completely inside if the test returns 1 and planeflags are zero
int Frustum::testBBox(bbox_t box, int &planeflags){
	int *pindex;
	vec3_t point;
	float dot;

	if(planeflags){		// FIXME: put that ouside from here.

		for(int i=0; i<num_planes; i++){
			if (!(planeflags & (1<<i))) continue;	// don't need to clip against it

			pindex = fixplanes[i].indexes;

			// int to float conversion isnt as nasty as float to int?
			point[0] = (float)box[pindex[0]];
			point[1] = (float)box[pindex[1]];
			point[2] = (float)box[pindex[2]];
			
			dot = DotProduct (point, planes[i].normal);
			dot -= planes[i].dist;

			if (dot <= 0) return 0;

			point[0] = (float)box[pindex[3+0]];
			point[1] = (float)box[pindex[3+1]];
			point[2] = (float)box[pindex[3+2]];

			dot = DotProduct (point, planes[i].normal);
			dot -= planes[i].dist;

			if (dot >= 0) planeflags &= ~(1<<i);	// node is entirely on screen
		}
	}
	return 1;
}


//
// test floating point bounding box, without clip planes (for entities, and patches)
//
int Frustum::testBBox( bboxf_t box ){
//	int *pindex;
//	vec3_t point;
//	float dot;

	/*for(int i=0; i<num_planes; i++){

		pindex = fixplanes[i].indexes;

		point[0] = box[pindex[0]];
		point[1] = box[pindex[1]];
		point[2] = box[pindex[2]];
		
		dot = DotProduct (point, planes[i].normal);
		dot -= planes[i].dist;

		if (dot <= 0) return 0;
	}*/

	for (int i=0; i<num_planes; i++){
		if (BoxOnPlaneSide (&box[0], &box[3], &planes[i]) == 2) return 0;
	}


	return 1;
}
