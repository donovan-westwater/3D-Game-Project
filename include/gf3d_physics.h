#ifndef __GF3D_PHYSICS_H__
#define __GF3D_PHYSICS_H__

#include "gfc_vector.h"
#include "gf3d_entity.h"
#include"gf3d_raycast.h"
#include "gfc_matrix.h"

#define rigidTotal entSize / 2
#define obsTotal entSize / 2
//Move to vectors
#define v3_scaler_mult(dst,v,b) (dst.x = b*v.x,dst.y = b*v.y,dst.z = b*v.z)
/*some geometrical structs for collsion*/
typedef struct Edge_S {
	Vector3D origin;
	Vector3D endpoint;
}Edge;
typedef struct Plane_S {
	Vector3D normal;
	float d; //distance from the origin
}Plane;

/*Physics Manager and rigidbody / obstacle definitions*/
typedef struct CollisionManifold_S {
	int colliding;
	Vector3D normal;
	float depth;
	Vector3D contacts[24];
	int contactSize;
} CollisionManifold;
typedef struct PhysicsSystem_S {
	
	Vector3D gravity; //Placeholder
	float friction; //Placeholder

	int rigidSize;
	int obsSize;
	Rigidbody rigidList[rigidTotal];
	Obstacle obsList[obsTotal];

	Rigidbody colliders1[100];
	Rigidbody colliders2[100];
	CollisionManifold results[100];
	int pairSize;
	float penetrationSlack;
	float linearProjectionPercent;
	int impulseIteration;

	

}PhysicsSystem;
static PhysicsSystem phyEngine = { 0 };
//** Physics system functions **//
void phyEngine_init();

PhysicsSystem* get_phyEngine();

void physicsUpdate(float time);

Rigidbody* addRigidbody(Rigidbody body);

Obstacle* addObstacle(Obstacle obs);

void clearRigidbodys();

Vector3D body_get_pos(Rigidbody* self);

void clearObstacles();
//Collision Manifold functions
void resetCollisionManifold(CollisionManifold* result);

//Both a and b must be spheres
CollisionManifold findCollisionSpheres(Entity* a, Entity* b);

//One of them is a box the other one is a sphere
CollisionManifold findCollisionSphereBox(Entity* a, Entity* b);

//Both a and b are boxes
CollisionManifold findCollisionBoxes(Entity* a, Entity* b);

CollisionManifold findCollisionFeatures(Entity* a, Entity* b);

void applyImpluse(Entity* a, Entity* b, CollisionManifold* m, int c);

//Rigidbody and constraint updates
void body_update(Rigidbody* self, float deltatime);

void body_ApplyForces(Rigidbody* self);

void body_solveObstacles(Rigidbody* self, Obstacle* obsList);

void body_addRotationalImpulse(Rigidbody* self,Vector3D point, Vector3D impulse);

Matrix4 *invTensor(Rigidbody *self);

/** SDF functions **/
/*Returns the signed distance the point is from a sphere whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
float sphereSDF(Vector3D p);

/*Returns the signed distance the point is from a box whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
/*Param: 3d vector to act as the bounds of the box*/
float boxSDF(Vector3D p, Vector3D b);

//**Geometry helper functions**//
void rotXb(Vector3D* o, float s, float c);
void rotYb(Vector3D* o, float s, float c);
void rotZb(Vector3D* o, float s, float c);
void rotX(Vector3D* o, float a);
void rotY(Vector3D* o, float a);
void rotZ(Vector3D* o, float a);

float radians(float d);


//returns all the vertices of the box
Vector3D* getVertices(Entity* a);

//returns all the edges of the box
Edge* getEdges(Entity* a);

//return all the planes of the box
Plane* getPlanes(Entity* a);

//returns true if clips plane interscets
int clipToPlane(Plane *plane, Edge *line, Vector3D *outPoint);

//Returns list of points that made contact
Vector3D* clipEdgesToBox(Edge* edges, Entity* a);

float penetrationDepth(Entity* a, Entity* b, Vector3D *axis, int *outShouldFlip);

//**INTERSECTION TESTS **//
//Trying to make a working oobb interection test syetm for the cubes
int obbTest(Entity *a, Entity *b);

int sphereOBBtest(Entity* a, Entity* b);

int sphereTest(Entity* a, Entity* b);

int isCollide(Entity* a, Entity* b);

int isInGround(Entity* a, Entity* b);

void collisionCheck(Entity *a, Entity *b);

void groundCheck(Entity* a);

#endif