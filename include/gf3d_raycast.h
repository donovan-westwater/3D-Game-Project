#include "gf3d_entity.h"
typedef struct Ray_s {
	Vector3D origin;
	Vector3D direction;
}Ray;

typedef struct RaycastResult_S {
	Vector3D point;
	Vector3D normal;
	float t;
	int hit;
}RaycastResult;

void resetRayResult(RaycastResult* outResult);

//Checks if a ray intersects a sphere (outresult opt via NULL) returns bool of if it hit or not
int raycastSphere(Entity* a, Ray* ray, RaycastResult* outResult);

//Checks if a ray intersects a OBB box (outresult opt via NULL) returns bool of if it hit or not
int raycastBox(Entity* a, Ray* ray, RaycastResult* outResult);

//Checks to see if ray interesects a plane- plane made out of given
//normal and the distnace from the origin (outresult opt via NULL) returns bool of if it hit or not
int raycastPlane(Vector3D n,float d, Ray* ray, RaycastResult* outResult);

int raycastGeneral(Entity* a, Ray* ray, RaycastResult* outResult);

int linetest(Entity* a, Vector3D posA, Vector3D posB);
