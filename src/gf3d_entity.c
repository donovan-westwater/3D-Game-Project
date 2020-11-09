#include "gf3d_vgraphics.h"
#include "gf3d_entity.h"
#include "gf3d_physics.h"

#define StepNum 5

static Entity entList[entSize] = { -1 };
static float count = 0;
//initilizes the list from the UBO and entity list
void initEntList() {

	for (int i = 0; i < entSize; i++) {
		entList[i].id = i;
		entList[i].update = update;
		entList[i].touch = entity_touch;
	}



}

Entity* addEntity(Vector4D pos, Vector4D rot, Vector4D scale, Vector4D color, Vector3D velo, int type,int isObs) {
	int i;
	for (i = 0; i < entSize; i++) {
		if (entList[i].inuse < 1) break;
	}
	if (i > entSize) {
		printf("OUT OF SPACE! NO ENTITY COULD BE CREATED");
	}
	UniformBufferObject* ubo = gf3d_get_pointer_to_UBO();
	 entList[i].rSelf = &ubo->renderList[i];
	entList[i].rSelf->position = pos;
	entList[i].rSelf->rotation = rot;
	entList[i].rSelf->scale = scale;
	entList[i].rSelf->color = color;
	entList[i].rSelf->id = i;
	if (isObs) {
		entList[i].isObs = isObs;
		Obstacle o = { 0 };
		o.eSelf = &entList[i];
		entList[i].obsSelf = addObstacle(o);
		//Fucntion pointers here!
	}
	else {
		entList[i].isObs = isObs;
		Rigidbody o = { 0 };
		o.eSelf = &entList[i];
		o.friction = 0.35; //.95
		o.gravity = vector3d(0, -1, 0);
		o.mass = 1;
		o.cor = 0.6f;
		o.bounce = 0.7f;

		o.oldPosition.x = entList[i].rSelf->position.x;
		o.oldPosition.y = entList[i].rSelf->position.y;
		o.oldPosition.z = entList[i].rSelf->position.z;
		
		o.addLinearImpluse = addLinearImpluse;
		o.addRotationalImpluse = body_addRotationalImpulse;
		o.solveObstacles = body_solveObstacles;
		o.update = rigidbody_update;
		o.applyForces = body_ApplyForces;
		o.invMass = invMass;
		entList[i].pSelf = addRigidbody(o);
		//Fucntion pointers here!
	}
	
    entList[i].inuse = 1;
	entList[i].velocity = velo;
	entList[i].rSelf->type = type;
	
	return &entList[i];
	
}

void entity_touch(Entity* self, Entity* other) {
	printf("WE HAVE COLLIDED!\n");
}

void updateEntAll() {
	for (int i = 0; i < entSize; i++) {
		if (entList[i].inuse < 1) continue;
		entList[i].update(&entList[i]);
	}
}

void update(Entity* self) {
	if (self->rSelf->position.y < -1) printf("Went throught floor!\n");
	/*
	count += 0.01;
	if (count > 500) count = 0;
	vector4d_add(self->rSelf->position, self->rSelf->position, vector4d(0, -0.0015 , 0, 0));
	Vector3D r = vector3d(self->rSelf->rotation.x, self->rSelf->rotation.y, self->rSelf->rotation.z);
	vector3d_rotate_about_y(&r, 0.1);
	self->rSelf->rotation.x = r.x;
	self->rSelf->rotation.y = r.y;
	self->rSelf->rotation.z = r.z;
	
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
		for (int i = 0; i < entSize; i++) {
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
	*/
	//check floor collision using col check on point at height of plane (i.e y = 0)
}

//Rigidbody functions

void rigidbody_applyforces(Rigidbody* self) {
	self->forces = vector3d(0, -9.8, 0);
}

float invMass(Rigidbody* self) {
	if (self->mass == 0) return 0;
	return 1 / self->mass;
}

void addLinearImpluse(Rigidbody* self, Vector3D impluse) {
	vector3d_add(self->eSelf->velocity, self->eSelf->velocity, impluse);
}

void rigidbody_update(Rigidbody* self, float time) {
	float damping = 0.98;
	Vector3D acc;
	v3_scaler_mult(acc, self->forces, self->invMass(self));
	vector3d_add(self->eSelf->velocity, self->eSelf->velocity, time * acc);
	v3_scaler_mult(self->eSelf->velocity, self->eSelf->velocity, damping);
	if (self->eSelf->rSelf->type == 1 || (self->eSelf->rSelf->scale.x != self->eSelf->rSelf->scale.z && self->eSelf->rSelf->scale.x != self->eSelf->rSelf->scale.y && self->eSelf->rSelf->scale.z != self->eSelf->rSelf->scale.y)){
		Vector4D out4;
		Vector4D in4;
		in4.x = self->torques.x;
		in4.y = self->torques.y;
		in4.z = self->torques.z;
		in4.w = 0;
		gfc_matrix_multiply_vector4d(&out4, invTensor(self), in4);
		Vector3D angAcc;
		angAcc.x = out4.x;
		angAcc.y = out4.y;
		angAcc.z = out4.z;
		float rad2deg = 180 / 3.1415;
		vector3d_add(self->angVel, self->angVel,time*angAcc);
		v3_scaler_mult(self->angVel, self->angVel, time);
		Vector3D orient = vector3d(self->eSelf->rSelf->rotation.x, self->eSelf->rSelf->rotation.y, self->eSelf->rSelf->rotation.z);
		vector3d_add(orient, orient, time * rad2deg*self->angVel);
		self->eSelf->rSelf->rotation.x = orient.x;
		self->eSelf->rSelf->rotation.y = orient.y;
		self->eSelf->rSelf->rotation.z = orient.z;
	}
	



	Vector3D pos = vector3d(self->eSelf->rSelf->position.x, self->eSelf->rSelf->position.y, self->eSelf->rSelf->position.z);
	vector3d_add(pos, pos, time * self->eSelf->velocity);
	self->eSelf->rSelf->position.x = pos.x;
	self->eSelf->rSelf->position.y = pos.y;
	self->eSelf->rSelf->position.z = pos.z;
}

//Collectibles
//Collectible add function goes here

void coll_update(Entity* self) {
	Vector3D pos = vector3d(self->rSelf->position.x, self->rSelf->position.y, self->rSelf->position.z);
	UniformBufferObject ubo = gf3d_vgraphics_get_uniform_buffer_object();
	Vector3D player = vector3d(ubo.view[3][0],ubo.view[3][1],ubo.view[3][2]);
	Vector3D line;
	vector3d_add(line, player, -pos);
	if (vector3d_magnitude(line) < 0.5 * self->rSelf->scale.x) {
		self->inuse = 0;
		self->rSelf->position = vector4d(-99999, -999999, -99999, 0);
		//Some player counter ticks up here
	}

}


