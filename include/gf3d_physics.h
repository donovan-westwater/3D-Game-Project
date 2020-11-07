#include "gfc_vector.h"
#include "gf3d_entity.h"
#include"gf3d_raycast.h"


#define rigidTotal entSize / 2
#define obsTotal entSize / 2
//Move to vectors
#define v3_scaler_mult(dst,v,b) (dst.x = b*v.x,dst.y = b*v.y,dst.z = b*v.z)
/*Physics Manager and rigidbody / obstacle definitions*/
typedef struct PhysicsSystem_S {
	
	Vector3D gravity; //Placeholder
	float friction; //Placeholder

	Rigidbody rigidList[rigidTotal];
	Obstacle obsList[obsTotal];
	int rigidSize;
	int obsSize;

}PhysicsSystem;
static PhysicsSystem phyEngine = { 0 };
//** Physics system functions **//
PhysicsSystem* get_phyEngine();

void physicsUpdate(float time);

Rigidbody* addRigidbody(Rigidbody body);

Obstacle* addObstacle(Obstacle obs);

void clearRigidbodys();

void clearObstacles();

//Rigidbody and constraint updates
void body_update(Rigidbody* self, float deltatime);

void body_ApplyForces(Rigidbody* self);

void body_solveObstacles(Rigidbody* self, Obstacle* obsList);

/** SDF functions **/
/*Returns the signed distance the point is from a sphere whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
float sphereSDF(Vector3D p);

/*Returns the signed distance the point is from a box whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
/*Param: 3d vector to act as the bounds of the box*/
float boxSDF(Vector3D p, Vector3D b);


//**INTERSECTION TESTS **//
//Trying to make a working oobb interection test syetm for the cubes
int obbTest(Entity *a, Entity *b);

int sphereOBBtest(Entity* a, Entity* b);

int sphereTest(Entity* a, Entity* b);

int isCollide(Entity* a, Entity* b);

int isInGround(Entity* a, Entity* b);

void collisionCheck(Entity *a, Entity *b);

void groundCheck(Entity* a);

