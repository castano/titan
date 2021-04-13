/*===========================================================================
	Title: player.h
	Author: Ignacio Castaño Aguado
	Description: player

===========================================================================*/
#ifndef _PLAYER_H__
#define _PLAYER_H__




class Player {
protected:
	Player(int type=CR_SPECTATOR);
	~Player(void);


// members:
protected:
	
	vec3_t velocity;

	Creature	*creature;

};





#endif	// _PLAYER_H__



