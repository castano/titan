/*===========================================================================
	Title: player.cpp
	Author: Ignacio Castaño Aguado
	Description: player

===========================================================================*/

#include "game logic.h"


/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern vars	*cvars;
extern Error *err;



Player::Player(int type){

	switch(type){
		case CR_SPECTATOR:
			creature=new Spectator();
		break;
		default:
			creature=NULL;
	};

	velocity[0]=0;
	velocity[1]=0;
	velocity[2]=0;

}


Player::~Player(void){

	delete creature; creature=NULL;

}