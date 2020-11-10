#include "gf3d_player.h"
#include <stdio.h>
static PlayerManger playerM = { 0 };
void playerManInit() {
	playerM.count = 0;
}


PlayerManger* get_PlayerManager() {
	return &playerM;
}

void playerUpdate(){
	if (playerM.count > 0) printf("I GOT A POINT!\n");
}
