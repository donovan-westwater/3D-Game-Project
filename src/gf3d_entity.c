#include "gf3d_vgraphics.h"
#include "gf3d_entity.h"


static Entity entList[50] = { -1 };

//initilizes the list from the UBO and entity list
void initEntList() {

	for (int i = 0; i < 50; i++) {
		entList[i].id = i;
		entList[i].update = update;
	}



}

void addEntity(Vector4D pos, Vector4D rot, Vector4D scale, Vector4D color, int type) {
	int i;
	for (i = 0; i < 51; i++) {
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
	entList[i].type = type;
	

	
}

void updateEntAll() {
	for (int i = 0; i < 51; i++) {
		if (entList[i].inuse < 1) continue;
		entList[i].update(&entList[i]);
	}
}

void update(Entity* self) {
	vector4d_add(self->rSelf->position, self->rSelf->position, vector4d(0.01, 0, 0, 0));
}












