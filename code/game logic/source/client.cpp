/*===========================================================================
	Title: client.cpp
	Author: Ignacio Castaño Aguado
	Description: client

===========================================================================*/

#include "game logic.h"


#define FRICTION		1000.0f


/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern vars	*cvars;
extern Error *err;
//extern Console *console;
extern IInput *input;
extern Timer *timer;




/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/


Client::Client(void):Player(){
	viewport_size=1.0f;
};


void Client::Init(void){
	max_width=cvars->IntForKey("v_width");
	max_height=cvars->IntForKey("v_height");

	if(max_width==0||max_height==0){	// there may be an error, use default res
		err->Log(UNKNOWN_ERROR, "--- uh, uh, something is going wrong\n");
		max_width=640;
		max_height=480;
	}
	
	float real_width=max_width*viewport_size;
	float real_height=max_height*viewport_size;

	cam.Init(creature->fov, real_width, real_height, max_width/2, max_height/2);
};


int Client::Update(void){
	vec3_t dir, versor={ 0.0f, 0.0f, 0.0f };
	vec3_t desp;

	if(input->GetValue(ID_KEYBOARD, KEY_ESCAPE)) return 0;

	if(input->GetValue(ID_KEYBOARD, KEY_UP)) versor[2]-=1.0f;
	if(input->GetValue(ID_KEYBOARD, KEY_DOWN)) versor[2]+=1.0f;

	if(input->GetValue(ID_KEYBOARD, KEY_LEFT)) versor[0]-=1.0f;
	if(input->GetValue(ID_KEYBOARD, KEY_RIGHT)) versor[0]+=1.0f;

	MultVect3x3(cam.Rotation, versor, dir);

	if(VectorNormalize(versor)){
		velocity[0]+=dir[0]*timer->frametime*creature->acceleration;
		velocity[1]+=dir[1]*timer->frametime*creature->acceleration;
		velocity[2]+=dir[2]*timer->frametime*creature->acceleration;
	}

	// calculamos la norma de la velocidad
	vec3_t temp;
	VectorCopy(velocity, temp);
	float vel=VectorNormalize(temp);

	velocity[0]-=temp[0]*timer->frametime*FRICTION;
	velocity[1]-=temp[1]*timer->frametime*FRICTION;
	velocity[2]-=temp[2]*timer->frametime*FRICTION;
	
	if( (temp[0]>0 && velocity[0]<0) ||
	    (temp[0]<0 && velocity[0]>0)) velocity[0]=0;
	if( (temp[1]>0 && velocity[1]<0) ||
		(temp[1]<0 && velocity[1]>0)) velocity[1]=0;
	if( (temp[2]>0 && velocity[2]<0) ||
		(temp[2]<0 && velocity[2]>0)) velocity[2]=0;


	if(vel>creature->max_speed) {
		float ivel=1/vel*creature->max_speed;
		VectorScale(velocity, ivel, velocity);
	}


	VectorScale(velocity, timer->frametime, desp);
	VectorAdd(desp, cam.c, cam.c);


	//if(input->GetValue(ID_KEYBOARD, DIK_GRAVE)) console->Toggle();

	cam.yaw+=(float) input->GetValue(ID_MOUSE, MV_XAXIS) * 0.25f;
	cam.pitch+=(float) input->GetValue(ID_MOUSE, MV_YAXIS) * 0.25f;

	// dont go out of bounds
	if(cam.pitch<0.0f)cam.pitch=0.0f;
	else if(cam.pitch>180.0f)cam.pitch=180.0f;

	return 1;
};