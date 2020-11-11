#include "gf3d_player.h"
#include "gf3d_entity.h"
#include "gf3d_vgraphics.h"
#include <stdio.h>
static PlayerManger playerM = { 0 };
void playerManInit() {
	playerM.count = 0;
	playerM.puzzle1 = addCollctible(vector3d(2, 1, 2));
	playerM.puzzle2 = addCollctible(vector3d(-2, 1, 2));
	playerM.puzzle3 = addCollctible(vector3d(2, 1, -2));
	//Initialize collectibes here
}


PlayerManger* get_PlayerManager() {
	return &playerM;
}

int get_RaycastAhead(RaycastResult* outResult) {
	//Create ray and test againest all entites in the scene
	Entity* entList = getEntList();
	UniformBufferObject *ubo = gf3d_get_pointer_to_UBO();
	RaycastResult result;
	resetRayResult(&result);
	RaycastResult test;
	Ray ahead;
	Vector4D forward = vector4d(0, 0, 1, 0);
	gfc_matrix_multiply_vector4d(&forward, ubo->view, forward);
	vector4d_normalize(&forward);
	ahead.direction = vector3d(forward.x,forward.y,forward.z);
	ahead.origin = vector3d(ubo->view[3][0], ubo->view[3][1], ubo->view[3][2]);
	float min = 99999;
	for (int i = 0; i < entSize; i++) {
		if (entList[i].inuse == 0) continue;
		if (entList[i].noCollide) continue;
		if (raycastGeneral(&entList[i], &ahead, &test)) {
			if (test.hit && test.t < min) {
				*outResult = test;
				min = test.t;
			}
		}

	}
	//return the result with lowest t
	return outResult->hit;
}


void playerUpdate(){
	if (playerM.count > 2) printf("I solved all the puzzles!\n");
}
