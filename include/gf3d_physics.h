#include "gfc_vector.h"
#include "gf3d_entity.h"

/** SDF functions **/
/*Returns the signed distance the point is from a sphere whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
float sphereSDF(Vector3D p);

/*Returns the signed distance the point is from a box whose model is at 0 0 0*/
/*Param: 3d vector to act the point you want to get the distance to*/
/*Param: 3d vector to act as the bounds of the box*/
float boxSDF(Vector3D p, Vector3D b);

//Trying to make a working oobb interection test syetm for the cubes
int oabbTest(Entity *a, Entity *b);

void collisionCheck(Entity a, Entity b);

void groundCheck(Entity* a);

