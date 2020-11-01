#include "gf3d_physics.h"

#define ZERO3D vector3d(0.0,0.0,0.0)
#define EST 0.001
/*HELPER FUNCTIONS*/
float length(Vector3D a) {
	return vector3d_magnitude(a);
}
float radians(float d) {
	float rad = 0.01745329251;
	return d * rad;
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
		return boxSDF(p,vector3d(0.25,0.25,0.25)); //this should match the values found in the rayMarch shader
		break;
	default:
		return sphereSDF(p);
		break;
	}
}
//Should check for collision and then adjust for said collision
void collisionCheck(Entity a, Entity b) {
	float d;
	Vector3D scale;
	Vector3D surfP;
	//Transform position of a into space of b
	Vector3D bSpace = vector3d(a.rSelf->position.x, a.rSelf->position.y, a.rSelf->position.z);
	//Use b entity rSelf parameters
	vector3d_add(bSpace, bSpace, -b.rSelf->position);
	rotZ(&bSpace, radians(b.rSelf->rotation.z));
	rotY(&bSpace, radians(b.rSelf->rotation.y));
	rotX(&bSpace, radians(b.rSelf->rotation.x));
	scale = vector3d(b.rSelf->scale.x, b.rSelf->scale.y, b.rSelf->scale.z);
	d = generalSDF(b.rSelf->type, vector3d(bSpace.x/scale.x, bSpace.y / scale.y, bSpace.z / scale.z))*min(scale.x, min(scale.y, scale.z));
	//Get the surface point on B
	Vector3D norm = vector3d(b.rSelf->position.x - a.rSelf->position.x, b.rSelf->position.y - a.rSelf->position.y, b.rSelf->position.z - a.rSelf->position.z);
	vector3d_normalize(&norm);
	surfP.x = a.rSelf->position.x + (norm.x) * d;
	surfP.y = a.rSelf->position.y + (norm.y) * d;
	surfP.z = a.rSelf->position.z + (norm.z) * d;
	//Transfrom surface point into space of A
	Vector3D aSpace = surfP;
	vector3d_add(aSpace, aSpace, -a.rSelf->position);
	rotZ(&aSpace, radians(a.rSelf->rotation.z));
	rotY(&aSpace, radians(a.rSelf->rotation.y));
	rotX(&aSpace, radians(a.rSelf->rotation.x));
	scale = vector3d(a.rSelf->scale.x, a.rSelf->scale.y, a.rSelf->scale.z);
	d = generalSDF(a.rSelf->type, vector3d(aSpace.x / scale.x, aSpace.y / scale.y, aSpace.z / scale.z)) * min(scale.x, min(scale.y, scale.z));
	//Check if distance is negative. if true, the collsion is true
	if (d <= 0) {
		//Create an adjustment vector to push out by using the distance function to get distance push for each axis
		Vector3D adj; //direction of adjustment relative to entity A
		//aSpace in the model space of A so we are testing the distance from the origin, and will return the relative direction
		Vector3D dir = vector3d(aSpace.x, 0, 0);
		vector3d_normalize(&dir);
		adj.x = -(1.0 + EST) * abs(generalSDF(a.rSelf->type, vector3d(aSpace.x, 0, 0))) * dir.x;
		dir = vector3d(0,aSpace.y, 0);
		vector3d_normalize(&dir);
		adj.y = -(1.0 + EST) * abs(generalSDF(a.rSelf->type, vector3d(0, aSpace.y, 0))) * dir.y;
		dir = vector3d(0, 0, aSpace.z);
		vector3d_normalize(&dir);
		adj.z = -(1.0+EST)*abs(generalSDF(a.rSelf->type, vector3d(0, 0, aSpace.z))) * dir.z;
		//adjusting the center of A with the relative adjustment vector
		vector3d_add(a.rSelf->position, a.rSelf->position, adj);
	}

}
//Physics Functions (Torque and Force functions)