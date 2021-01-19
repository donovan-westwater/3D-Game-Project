#include "gf3d_player.h"
#include "gf3d_entity.h"
#include "gf3d_vgraphics.h"
#include <stdio.h>
static PlayerManger playerM = { 0 };
void playerManInit() {
	playerM.count = 0;
	playerM.isCollide = true;
	playerM.hasLost = false;
	playerM.puzzle1 = addCollctible(vector3d(0, 1, 8));
	playerM.puzzle2 = addCollctible(vector3d(2, 1, 2));
	playerM.puzzle3 = addCollctible(vector3d(4, 1, 8));
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
	resetRayResult(&test);
	Ray ahead;
	Vector4D forward = vector4d(0, 0, 1, 0);
	gfc_matrix_multiply_vector4d(&forward, ubo->view, forward);
	vector4d_normalize(&forward);
	ahead.direction = vector3d(forward.x,forward.y,forward.z);
	ahead.origin = vector3d(ubo->view[3][0], ubo->view[3][1], ubo->view[3][2]);
	float min = 99999;
	for (int i = 0; i < entSize; i++) {
		if (entList[i].inuse == 0) continue;
		if (entList[i].noCollide || !playerM.isCollide ||entList[i].eType != Wall) continue;
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
//New Idea: Raycast in a fan in front of the player for walls
//REDO THIS FUNCTION (SEE ABOVE)
int lineOfplayer() {
	Entity* entList = getEntList();
	UniformBufferObject* ubo = gf3d_get_pointer_to_UBO();
	RaycastResult result;
	resetRayResult(&result);
	RaycastResult test;
	Ray ahead;
	Vector3D playerPos = vector3d(ubo->view[3][0], ubo->view[3][1], ubo->view[3][2]);
	Vector3D dir;
	Vector3D entPos;
	Vector3D playDist;
	float min = 99999;
	for (int i = 0; i < entSize; i++) {
		if (entList[i].inuse == 0 || entList[i].rSelf->type == 2|| entList[i].rSelf->type == 3) continue;
		entPos = vector3d(entList[i].rSelf->position.x, entList[i].rSelf->position.y, entList[i].rSelf->position.z);
		vector3d_sub(dir, entPos, playerPos);
		playDist = dir;
		vector3d_normalize(&dir);
		ahead.direction = vector3d(dir.x, dir.y, dir.z);
		ahead.origin = vector3d(ubo->view[3][0], ubo->view[3][1], ubo->view[3][2]);
		if (raycastGeneral(&entList[i], &ahead, &test)) {
			result = test;
		}
		else result.point = vector3d(99999, 999999, 999999);
		Vector3D distFromSelf;
		vector3d_sub(distFromSelf, result.point, entPos);
		//if dist to the wall that is hit is less than the dist to the player, then there is no line of sight present
		if (vector3d_magnitude(playDist) < vector3d_magnitude(distFromSelf)) entList[i].rSelf->id = -1;
		else entList[i].rSelf->id = entList[i].rSelf->id;
	}

}

void playerUpdate(){
	//lineOfplayer();
	if (playerM.count > 2) printf("I solved all the puzzles!\n");
}
