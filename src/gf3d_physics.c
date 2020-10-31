#include "gf3d_physics.h"

#define ZERO3D vector3d(0.0,0.0,0.0)

/*HELPER FUNCTIONS*/
float length(Vector3D a) {
	return vector3d_magnitude(a);
}
/*
float length(Vector4D a) {
	return vector4d_magnitude(a);
}
*/
Vector3D VectorMax(Vector3D a, Vector3D b) {
	return vector3d(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}
Vector3D VectorMin(Vector3D a, Vector3D b) {
	return vector3d(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
/*
Vector4D VectorMax(Vector4D a, Vector4D b) {
	return vector4d(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}
Vector4D VectorMin(Vector4D a, Vector4D b) {
	return vector4d(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
}
*/
/*Transformation fuctions*/
void rotXb(Vector3D *o, float s, float c) {
	Vector2D rotX = vector2d(c * o->y + s * o->z, c * o->z - s * o->y);
	o->y = rotX.x;
	o->z = rotX.y;
}
void rotYb(Vector3D* o, float s, float c) {
	Vector2D rotY = vector2d(c * o->x - s * o->z, c * o->z + s * o->x);
	o->x = rotY.x;
	o->z = rotY.y;
}
void rotZb(Vector3D* o, float s, float c) {
	Vector2D rotZ = vector2d(c * o->x + s * o->y, c * o->y - s * o->x);
	o->x = rotZ.x;
	o->z = rotZ.y;
}
void rotX(Vector3D* o, float a) {
	rotXb(o, sin(a), cos(a));
}
void rotY(Vector3D* o, float a) {
	rotYb(o, sin(a), cos(a));
}
void rotZ(Vector3D* o, float a) {
	rotZb(o, sin(a), cos(a));
}
/*Signed Distance Functions*/
float sphereSDF(Vector3D p) {
	return length(p) - 0.5; //hard coded to match frag shader
}

float boxSDF(Vector3D p, Vector3D b) {
	Vector3D a = vector3d(abs(p.x), abs(p.y), abs(p.z));
	Vector3D q;
	vector2d_add(q, a, -b);
	return length(VectorMax(q,ZERO3D)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
//Points should be transformed before they are passed into general
float generalSDF(int type, Vector3D p) {
	switch (type)
	{
	case 0:
		
		return sphereSDF(p);
		break;
	case 1:
		return boxSDF(p,vector3d(1,1,1)); //this should match the values found in the rayMarch shader
		break;
	default:
		return sphereSDF(p);
		break;
	}
}
void collisionCheck(Entity a, Entity b) {
	float d;
	//Transform position of a into space of b
	Vector3D bSpace = vector3d(a.rSelf->position.x, a.rSelf->position.y, a.rSelf->position.z);
	

}