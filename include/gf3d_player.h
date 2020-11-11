#ifndef __GF3D_PLAYER_H__
#define __GF3D_PLAYER_H__
#include "gf3d_raycast.h"
typedef struct PlayerManger_S {
	int count;
	Entity* puzzle1;
	Entity* puzzle2;
	Entity* puzzle3;

}PlayerManger;
//Move into c file and make a function to return pointer to this sturct!


void playerUpdate();

PlayerManger* get_PlayerManager();

int get_RaycastAhead(RaycastResult* outResult);

void playerManInit();
#endif