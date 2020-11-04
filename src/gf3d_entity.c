#include "gf3d_vgraphics.h"
#include "gf3d_entity.h"
#include "gf3d_physics.h"

static Entity entList[50] = { -1 };
static float count = 0;
//initilizes the list from the UBO and entity list
void initEntList() {

	for (int i = 0; i < 50; i++) {
		entList[i].id = i;
		entList[i].update = update;
	}



}

void addEntity(Vector4D pos, Vector4D rot, Vector4D scale, Vector4D color, Vector3D velo, int type) {
	int i;
	for (i = 0; i < 50; i++) {
		if (entList[i].inuse < 1) break;
	}
	if (i > 50) {
		printf("OUT OF SPACE! NO ENTITY COULD BE CREATED");
	}
	UniformBufferObject* ubo = gf3d_get_pointer_to_UBO();
	 entList[i].rSelf = &ubo->renderList[i];
	entList[i].rSelf->position = pos;
	entList[i].rSelf->rotation = rot;
	entList[i].rSelf->scale = scale;
	entList[i].rSelf->color = color;
	entList[i].rSelf->id = i;
	entList[i].inuse = 1;
	entList[i].velocity = velo;
	entList[i].rSelf->type = type;
	

	
}

void updateEntAll() {
	for (int i = 0; i < 50; i++) {
		if (entList[i].inuse < 1) continue;
		entList[i].update(&entList[i]);
	}
}

void update(Entity* self) {
	/*
	count += 0.01;
	if (count > 500) count = 0;
	vector4d_add(self->rSelf->position, self->rSelf->position, vector4d(0, -0.0015 , 0, 0));
	Vector3D r = vector3d(self->rSelf->rotation.x, self->rSelf->rotation.y, self->rSelf->rotation.z);
	vector3d_rotate_about_y(&r, 0.1);
	self->rSelf->rotation.x = r.x;
	self->rSelf->rotation.y = r.y;
	self->rSelf->rotation.z = r.z;
	*/
	vector4d_add(self->rSelf->position, self->rSelf->position,vector4d(self->velocity.x, self->velocity.y, self->velocity.z,0));
	groundCheck(self);
	
	//check floor collision using col check on point at height of plane (i.e y = 0)
}












