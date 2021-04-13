/*===========================================================================
	Title: client.h
	Author: Ignacio Castaño Aguado
	Description: client

===========================================================================*/
#ifndef _CLIENT_H__
#define _CLIENT_H__


#include "rfrontend.h"


/*---------------------------------------------------------------------------
	Actions:
---------------------------------------------------------------------------*/

enum {
	MOVE_FORWARD,
	MOVE_BACKWARD,
	STRAFE_LEFT,
	STRAFE_RIGHT,
	TURN_LEFT,
	TURN_RIGHT,
	LOOK_UP,
	LOOK_DOWN,
	JUMP,
	FIRE,
	CROUCH,
	// ...
};



/*---------------------------------------------------------------------------
	Client:
---------------------------------------------------------------------------*/


class Client : public Player {
public:
	Client(void);
	
	void Init(void);
	void Shut(void);

	int Update(void);

//private:

	Camera cam;

private:
	float	viewport_size;		// normalized
	float	max_width;
	float	max_height;


};





#endif	// _CLIENT_H__




