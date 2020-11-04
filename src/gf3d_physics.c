#include "gf3d_physics.h"
#include "gfc_matrix.h"
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
	o->y = rotZ.y;
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
	Vector3D q = vector3d(0, 0, 0);
	vector2d_add(q, a, -b);
	return length(VectorMax(q, ZERO3D)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
//Points should be transformed before they are passed into general
float generalSDF(int type, Vector3D p) {
	switch (type)
	{
	case 0:

		return sphereSDF(p);
		break;
	case 1:
		return boxSDF(p, vector3d(0.25, 0.25, 0.25)); //this should match the values found in the rayMarch shader
		break;
	default:
		return sphereSDF(p);
		break;
	}
}
//a and b must be sphers
int sphereTest(Entity* a, Entity* b) {
	Vector3D dir = vector3d(b->rSelf->position.x - a->rSelf->position.x, b->rSelf->position.y - a->rSelf->position.y, b->rSelf->position.z - a->rSelf->position.z);
	float d = dir.x*dir.x+dir.y*dir.y+dir.z*dir.z;
	float radSum = (a->rSelf->scale.x * 0.5 + b->rSelf->scale.x * 0.5);
	if (radSum * radSum < d) return 0;
	else return 1;


}
//works only if both a and b are obb's
int obbTest(Entity* a, Entity* b) {
	Vector3D ba = vector3d(0.25 * a->rSelf->scale.x, 0.25 * a->rSelf->scale.y, 0.25 * a->rSelf->scale.z);
	Vector3D bb = vector3d(0.25 * b->rSelf->scale.x, 0.25 * b->rSelf->scale.y, 0.25 * b->rSelf->scale.z);;
	Vector3D aMat[3];
	Vector3D bMat[3];
	//creating local axis for a and b
	aMat[0] = vector3d(1, 0, 0);
	bMat[0] = vector3d(1, 0, 0);

	aMat[1] = vector3d(0, 1, 0);
	bMat[1] = vector3d(0, 1, 0);

	aMat[2] = vector3d(0, 0, 1);
	bMat[2] = vector3d(0, 0, 1);

	//rotating them into position
	for (int i = 0; i < 3; i++) {
		rotX(&aMat[i], -a->rSelf->rotation.x);
		rotY(&aMat[i], -a->rSelf->rotation.y);
		rotZ(&aMat[i], -a->rSelf->rotation.z);

		rotX(&bMat[i], -b->rSelf->rotation.x);
		rotY(&bMat[i], -b->rSelf->rotation.y);
		rotZ(&bMat[i], -b->rSelf->rotation.z);

	}
	float ra, rb;
	Vector3D matR[3];
	Vector3D absR[3];
	for (int i = 0; i < 3; i++) {
		matR[i].x = vector3d_dot_product(aMat[i], bMat[0]);
		matR[i].y = vector3d_dot_product(aMat[i], bMat[1]);
		matR[i].z = vector3d_dot_product(aMat[i], bMat[2]);
	}
	Vector3D t = vector3d(a->rSelf->position.x - b->rSelf->position.x, a->rSelf->position.y - b->rSelf->position.y, a->rSelf->position.z - b->rSelf->position.z);
	t = vector3d(vector3d_dot_product(t, aMat[0]), vector3d_dot_product(t, aMat[1]), vector3d_dot_product(t, aMat[2]));
	for (int i = 0; i < 3; i++) {
		absR[i].x = fabsf(matR[i].x) + 0.01;
		absR[i].y = fabsf(matR[i].y) + 0.01;
		absR[i].z = fabsf(matR[i].z) + 0.01;
	}
	//test L = A0
	for (int i = 0; i < 3; i++) {
		rb = bb.x * absR[i].x + bb.y * absR[i].y + bb.z * absR[i].z;
		switch (i)
		{
		case 0:
			ra = ba.x;
			if (fabsf(t.x) > ra + rb) return 0;
			break;
		case 1:
			ra = ba.y;
			if (fabsf(t.y) > ra + rb) return 0;
			break;
		case 2:
			ra = ba.z;
			if (fabsf(t.z) > ra + rb) return 0;
			break;

		}
	}
	//test L = B0
	ra = ba.x * absR[0].x + ba.y * absR[1].x + ba.z * absR[2].x;
	rb = bb.x;
	if (fabsf(t.x * matR[0].x + t.y * matR[1].x + t.z * matR[2].x) > ra + rb){
		return 0;
	}

	ra = ba.x * absR[0].y + ba.y * absR[1].y + ba.z * absR[2].y;
	rb = bb.y;
	if (fabsf(t.x * matR[0].y + t.y * matR[1].y + t.z * matR[2].y) > ra + rb) {
		return 0;
	}

	ra = ba.x * absR[0].z + ba.y * absR[1].z + ba.z * absR[2].z;
	rb = bb.z;
	if (fabsf(t.x * matR[0].z + t.y * matR[1].z + t.z * matR[2].z) > ra + rb) {
		return 0;
	}
	//Test axis L = A0 x B0
	ra = ba.y * absR[2].x + ba.z * absR[1].x;
	rb = bb.y * absR[0].z + bb.z * absR[0].y;
	if (fabsf(t.z * matR[1].x - t.y * matR[2].x) > ra + rb) return 0;

	//Test axis L = A0 x B1
	ra = ba.y * absR[2].y + ba.z * absR[1].y;
	rb = bb.x * absR[0].z + bb.z * absR[0].x;
	if (fabsf(t.z * matR[1].y - t.y * matR[2].y) > ra + rb) return 0;
	
	//Test axis L = A0 x B2
	ra = ba.y * absR[2].z + ba.z * absR[1].z;
	rb = bb.x * absR[0].x + bb.y * absR[0].x;
	if (fabsf(t.z * matR[1].z - t.y * matR[2].z) > ra + rb) return 0;

	//Test axis L = A1 x B0
	ra = ba.x * absR[2].x + ba.z * absR[0].x;
	rb = bb.y * absR[1].z + bb.z * absR[1].y;
	if (fabsf(t.x * matR[2].x - t.z * matR[0].z) > ra + rb) return 0;

	//Test axis L = A1 x B1
	ra = ba.x * absR[2].y + ba.z * absR[0].y;
	rb = bb.x * absR[1].z + bb.z * absR[1].x;
	if (fabsf(t.x * matR[2].y - t.z * matR[0].y) > ra + rb) return 0;

	//Test axis L = A1 x B2
	ra = ba.x * absR[2].z + ba.z * absR[0].y;
	rb = bb.x * absR[1].y + bb.y * absR[1].x;
	if (fabsf(t.x * matR[2].z - t.z * matR[0].z) > ra + rb) return 0;

	//Test axis L = A2 x B0
	ra = ba.x * absR[1].x + ba.y * absR[0].x;
	rb = bb.y * absR[2].z + bb.z * absR[2].y;
	if (fabsf(t.y * matR[0].x - t.x * matR[1].x) > ra + rb) return 0;

	//Test axis L = A2 x B1
	ra = ba.x * absR[1].y + ba.y * absR[0].y;
	rb = bb.x * absR[2].z + bb.z * absR[2].x;
	if (fabsf(t.y * matR[0].y - t.x * matR[1].y) > ra + rb) return 0;
	
	//Test axis L = A2 x B2
	ra = ba.x * absR[1].z + ba.y * absR[0].z;
	rb = bb.x * absR[2].z + bb.y * absR[2].x;
	if (fabsf(t.y * matR[0].z - t.x * matR[1].z) > ra + rb) return 0;

	return 1;
}	
//one of them is a sphere, one of them is a OBB
int sphereOBBtest(Entity* a, Entity* b) {
	Vector3D point;
	Vector3D scale;
	float r = 0;
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		point = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
		vector3d_add(point, point, -b->rSelf->position);
		rotZ(&point, radians(b->rSelf->rotation.z));
		rotY(&point, radians(b->rSelf->rotation.y));
		rotX(&point, radians(b->rSelf->rotation.x));
		scale = vector3d(b->rSelf->scale.x, b->rSelf->scale.y, b->rSelf->scale.z);
		r = 0.5*a->rSelf->scale.x;
	}
	else if(b->rSelf->type == 0 && b->rSelf->scale.x == b->rSelf->scale.z && b->rSelf->scale.x == b->rSelf->scale.y && b->rSelf->scale.z == b->rSelf->scale.y) {
		point = vector3d(b->rSelf->position.x, b->rSelf->position.y, b->rSelf->position.z);
		vector3d_add(point, point, -a->rSelf->position);
		rotZ(&point, radians(a->rSelf->rotation.z));
		rotY(&point, radians(a->rSelf->rotation.y));
		rotX(&point, radians(a->rSelf->rotation.x));
		scale = vector3d(a->rSelf->scale.x, a->rSelf->scale.y, a->rSelf->scale.z);
		r = 0.5*b->rSelf->scale.x;
	}
	else {
		return -1;
	}
	float d = 0;
	if (point.x < -0.25 * scale.x) d += (point.x - 0.25 * scale.x);
	else if(point.x > 0.25 * scale.x) d += (point.x + 0.25 * scale.x);
	
	if (point.y < -0.25 * scale.y) d += (point.y - 0.25 * scale.y);
	else if (point.y > 0.25 * scale.y) d += (point.y + 0.25 * scale.y);

	if (point.z < -0.25 * scale.z) d += (point.z - 0.25 * scale.z);
	else if (point.z > 0.25 * scale.z) d += (point.z + 0.25 * scale.z);
	
	if (d <= r * r) return 1;
	else return 0;
}
int isCollide(Entity* a, Entity* b) {
	int sphereFlag = 1;
	int aFlag = 0;
	int bFlag = 0;
	if (a->rSelf->type == 0 || b->rSelf->type == 0) {
		if (fabsf(a->rSelf->scale.x - a->rSelf->scale.z) < 0.01  && fabsf(a->rSelf->scale.x - a->rSelf->scale.y) < 0.01 && fabsf(a->rSelf->scale.z - a->rSelf->scale.y) < 0.01) {
			aFlag = 1;
		}
		if (fabsf(b->rSelf->scale.x - b->rSelf->scale.z) < 0.01 && fabsf(b->rSelf->scale.x - b->rSelf->scale.y) < 0.01 && fabsf(b->rSelf->scale.z - b->rSelf->scale.y) < 0.01) {
			bFlag = 1;
		}
		if (aFlag && bFlag) {
			return sphereTest(a, b);
		}
	}
	if (aFlag || bFlag)
	{
		return sphereOBBtest(a, b);
	}
	else {
		return obbTest(a, b);
	}
}

int isInGround(Entity* a, Entity* b) {
	Vector3D n = vector3d(0, 1, 0);
	Vector3D c = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D e = vector3d(0.25 * a->rSelf->scale.x, 0.25 * a->rSelf->scale.y, 0.25 * a->rSelf->scale.z);
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		if (a->rSelf->position.y < a->rSelf->scale.x * 0.5) return 1;
	}
	else {
		if (a->rSelf->type == 0) {
			e = vector3d(0.5 * a->rSelf->scale.x, 0.5 * a->rSelf->scale.y, 0.5 * a->rSelf->scale.z);
		}
		//Vector3D pv = vector3d(-a->rSelf->position.x, -a->rSelf->position.y, -a->rSelf->position.z);
		rotX(&n, a->rSelf->rotation.x);
		rotY(&n, a->rSelf->rotation.y);
		rotZ(&n, a->rSelf->rotation.z);
		vector3d_normalize(&n);
		//rotX(&pv, a->rSelf->rotation.x);
		//rotY(&pv, a->rSelf->rotation.y);
		//rotZ(&pv, a->rSelf->rotation.z);

		float r = e.x * fabsf(n.x) + e.y * fabsf(n.y) + e.z * fabsf(n.z);
		//float pd = vector3d_magnitude(pv);
		float s = c.y;

		if (fabsf(s) <= r) {
			return 1;
		}
		return 0;
	}
}

//Should check for collision and then adjust for said collision [may not use]
void collisionCheck(Entity *a, Entity *b) {
	int sphereFlag = 1;
	int aFlag = 1;
	int bFlag = 1;
	if (a->rSelf->type == 0 && b->rSelf->type == 0) {
		if (a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
			aFlag = 0;
		}
		if (b->rSelf->scale.x == b->rSelf->scale.z && b->rSelf->scale.x == b->rSelf->scale.y && b->rSelf->scale.z == b->rSelf->scale.y) {
			bFlag = 0;
		}
		if (aFlag && bFlag) {
			if (sphereTest(a, b)) {
				return;
			}
		}
	}
}

//Checks if the entity has collied with the ground (y = 0) and adjusts entity out of collision
//Remake, for spheres test to see if the radius is less than p.y
//For 
void groundCheck(Entity *a) {
	Vector3D n = vector3d(0, 1, 0);
	Vector3D c = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D e = vector3d(0.25 * a->rSelf->scale.x, 0.25 * a->rSelf->scale.y, 0.25 * a->rSelf->scale.z);
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		if (a->rSelf->position.y < a->rSelf->scale.x * 0.5) a->rSelf->position.y += (a->rSelf->scale.x * 0.5 - a->rSelf->position.y);
		return;
	}
	else {
		if (a->rSelf->type == 0) {
			e = vector3d(0.5 * a->rSelf->scale.x, 0.5 * a->rSelf->scale.y, 0.5 * a->rSelf->scale.z);
		}
		//Vector3D pv = vector3d(-a->rSelf->position.x, -a->rSelf->position.y, -a->rSelf->position.z);
		rotX(&n, a->rSelf->rotation.x);
		rotY(&n, a->rSelf->rotation.y);
		rotZ(&n, a->rSelf->rotation.z);
		vector3d_normalize(&n);
		//rotX(&pv, a->rSelf->rotation.x);
		//rotY(&pv, a->rSelf->rotation.y);
		//rotZ(&pv, a->rSelf->rotation.z);

		float r = e.x * fabsf(n.x) + e.y * fabsf(n.y) + e.z * fabsf(n.z);
		//float pd = vector3d_magnitude(pv);
		float s = c.y;

		if (fabsf(s) <= r) {
			a->rSelf->position.y += fabsf((r - fabsf(s)));
		}
		return;
	}
}
//Physics Functions (Torque and Force functions)



/*
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

*/
