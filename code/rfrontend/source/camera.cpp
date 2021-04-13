/*===========================================================================
	Title: camera.cpp
	Author: Ignacio Castaño Aguado
	Description: Camera class, it does currently nothing at all

===========================================================================*/
#include "rfrontend.h"	// main header of the frontend files





/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IOutputDevice *out;



/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

void Camera::Init(float fov, float w, float h, float x, float y){
	out->Insert("--- Initializing Camera\n");

	// convert fov to radians
	this->fov=fov*3.14159f/180.0f;
	cos_fov=(float)FastCos(fov/2.0f);
		

	width=w;
	height=h;


	// Projection ratio
	proj_ratio=w/h;

	c[0]=0;
	c[1]=0;
	c[2]=10;

	yaw=0;
	pitch=90;
	roll=0;

};
