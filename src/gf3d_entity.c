#include "gf3d_vgraphics.h"
#include "gf3d_entity.h"
#include "gf3d_physics.h"

#define StepNum 5
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
	float numOfSteps = 0;
	for (float i = 1; i <= StepNum; i++) {
		Vector4D p0 = vector4d(self->rSelf->position.x, self->rSelf->position.y, self->rSelf->position.z,1);
		Vector4D v = vector4d((i / StepNum) * self->velocity.x, (i / StepNum) * self->velocity.y, (i / StepNum) * self->velocity.z,0);
		Vector4D ahead;
		vector4d_add(ahead, p0, v);
		Entity test = *self;
		EntityRender rTest = *test.rSelf;
		test.rSelf = &rTest;
		test.rSelf->position = ahead;
		int out = 0;
		for (int i = 0; i < 50; i++) {
			if (entList[i].inuse < 1) continue;
			if (test.rSelf->id == i) continue;
			if (isCollide(&test, &entList[i])) {
				out = 1;
				break;
			}
		}
		if (out) break;
		numOfSteps++;
	}
	vector4d_add(self->rSelf->position, self->rSelf->position,vector4d((numOfSteps / StepNum) * self->velocity.x, (numOfSteps / StepNum) * self->velocity.y, (numOfSteps / StepNum) * self->velocity.z,0));
	groundCheck(self);
	
	//check floor collision using col check on point at height of plane (i.e y = 0)
}












