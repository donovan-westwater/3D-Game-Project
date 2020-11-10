#ifndef __GF3D_PLAYER_H__
#define __GF3D_PLAYER_H__

typedef struct PlayerManger_S {
	int count;

}PlayerManger;
//Move into c file and make a function to return pointer to this sturct!


void playerUpdate();

PlayerManger* get_PlayerManager();

void playerManInit();
#endif