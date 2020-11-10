#include "gf3d_physics.h"


#define ZERO3D vector3d(0.0,0.0,0.0)
#define EST 0.001

static Vector3D vertices[8] = { 0 };
static Edge edges[12] = { 0 };
static Plane planes[6] = { 0 };
static Vector3D contPoints[12] = { 0 };
static int contSize = 0;
static Matrix4 tensor = { 0 };

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
Vector2D getInterval(Entity* a, Vector3D axis) {
	Vector3D c = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D e = vector3d(0.25*a->rSelf->scale.x, 0.25 * a->rSelf->scale.y, 0.25 * a->rSelf->scale.z);
	if(a->rSelf->type == 0) e = vector3d(0.5 * a->rSelf->scale.x, 0.5 * a->rSelf->scale.y, 0.5 * a->rSelf->scale.z);
	Vector3D aMat[3];
	Vector3D boxV[8];
	//creating local axis for a
	aMat[0] = vector3d(1, 0, 0);

	aMat[1] = vector3d(0, 1, 0);

	aMat[2] = vector3d(0, 0, 1);

	//rotating them into position
	for (int i = 0; i < 3; i++) {
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));
	}
	//Populating vertices [This is going to be very long!]
	boxV[0].x = c.x + aMat[0].x * e.x + aMat[1].x * e.y + aMat[2].x * e.z;
	boxV[0].y = c.y + aMat[0].y * e.x + aMat[1].y * e.y + aMat[2].y * e.z;
	boxV[0].z = c.z + aMat[0].z * e.x + aMat[1].z * e.y + aMat[2].z * e.z;

	boxV[1].x = c.x - aMat[0].x * e.x + aMat[1].x * e.y + aMat[2].x * e.z;
	boxV[1].y = c.y - aMat[0].y * e.x + aMat[1].y * e.y + aMat[2].y * e.z;
	boxV[1].z = c.z - aMat[0].z * e.x + aMat[1].z * e.y + aMat[2].z * e.z;

	boxV[2].x = c.x + aMat[0].x * e.x - aMat[1].x * e.y + aMat[2].x * e.z;
	boxV[2].y = c.y + aMat[0].y * e.x - aMat[1].y * e.y + aMat[2].y * e.z;
	boxV[2].z = c.z + aMat[0].z * e.x - aMat[1].z * e.y + aMat[2].z * e.z;

	boxV[3].x = c.x + aMat[0].x * e.x + aMat[1].x * e.y - aMat[2].x * e.z;
	boxV[3].y = c.y + aMat[0].y * e.x + aMat[1].y * e.y - aMat[2].y * e.z;
	boxV[3].z = c.z + aMat[0].z * e.x + aMat[1].z * e.y - aMat[2].z * e.z;

	boxV[4].x = c.x - aMat[0].x * e.x - aMat[1].x * e.y - aMat[2].x * e.z;
	boxV[4].y = c.y - aMat[0].y * e.x - aMat[1].y * e.y - aMat[2].y * e.z;
	boxV[4].z = c.z - aMat[0].z * e.x - aMat[1].z * e.y - aMat[2].z * e.z;

	boxV[5].x = c.x + aMat[0].x * e.x - aMat[1].x * e.y - aMat[2].x * e.z;
	boxV[5].y = c.y + aMat[0].y * e.x - aMat[1].y * e.y - aMat[2].y * e.z;
	boxV[5].z = c.z + aMat[0].z * e.x - aMat[1].z * e.y - aMat[2].z * e.z;

	boxV[6].x = c.x - aMat[0].x * e.x + aMat[1].x * e.y - aMat[2].x * e.z;
	boxV[6].y = c.y - aMat[0].y * e.x + aMat[1].y * e.y - aMat[2].y * e.z;
	boxV[6].z = c.z - aMat[0].z * e.x + aMat[1].z * e.y - aMat[2].z * e.z;

	boxV[7].x = c.x - aMat[0].x * e.x - aMat[1].x * e.y + aMat[2].x * e.z;
	boxV[7].y = c.y - aMat[0].y * e.x - aMat[1].y * e.y + aMat[2].y * e.z;
	boxV[7].z = c.z - aMat[0].z * e.x - aMat[1].z * e.y + aMat[2].z * e.z;

	//Min - x    Max - y
	Vector2D result;
	result.x = result.y = vector3d_dot_product(axis, boxV[0]);
	for (int i = 1; i < 8; ++i) {
		float projection = vector3d_dot_product(axis,boxV[i]);
		result.x = (projection < result.x)
			?
			projection : result.x;
		result.y = (projection > result.y)
			?
			projection : result.y;
	}
	return result;

}
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

//**Geometry helper functions**//
Vector3D* getVertices(Entity* a) {
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		printf("NOT A BOX!\n");
		return;
	}
	Vector3D c = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D e = vector3d(a->rSelf->scale.x, a->rSelf->scale.y, a->rSelf->scale.z);
	Vector3D aMat[3];
	//creating local axis for a
	aMat[0] = vector3d(1, 0, 0);

	aMat[1] = vector3d(0, 1, 0);

	aMat[2] = vector3d(0, 0, 1);

	//rotating them into position
	for (int i = 0; i < 3; i++) {
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));
	}
	//Populating vertices [This is going to be very long!]
	vertices[0].x = c.x + aMat[0].x * e.x + aMat[1].x * e.y + aMat[2].x * e.z;
	vertices[0].y = c.y + aMat[0].y * e.x + aMat[1].y * e.y + aMat[2].y * e.z;
	vertices[0].z = c.z + aMat[0].z * e.x + aMat[1].z * e.y + aMat[2].z * e.z;
	
	vertices[1].x = c.x - aMat[0].x * e.x + aMat[1].x * e.y + aMat[2].x * e.z;
	vertices[1].y = c.y - aMat[0].y * e.x + aMat[1].y * e.y + aMat[2].y * e.z;
	vertices[1].z = c.z - aMat[0].z * e.x + aMat[1].z * e.y + aMat[2].z * e.z;
	
	vertices[2].x = c.x + aMat[0].x * e.x - aMat[1].x * e.y + aMat[2].x * e.z;
	vertices[2].y = c.y + aMat[0].y * e.x - aMat[1].y * e.y + aMat[2].y * e.z;
	vertices[2].z = c.z + aMat[0].z * e.x - aMat[1].z * e.y + aMat[2].z * e.z;
	
	vertices[3].x = c.x + aMat[0].x * e.x + aMat[1].x * e.y - aMat[2].x * e.z;
	vertices[3].y = c.y + aMat[0].y * e.x + aMat[1].y * e.y - aMat[2].y * e.z;
	vertices[3].z = c.z + aMat[0].z * e.x + aMat[1].z * e.y - aMat[2].z * e.z;

	vertices[4].x = c.x - aMat[0].x * e.x - aMat[1].x * e.y - aMat[2].x * e.z;
	vertices[4].y = c.y - aMat[0].y * e.x - aMat[1].y * e.y - aMat[2].y * e.z;
	vertices[4].z = c.z - aMat[0].z * e.x - aMat[1].z * e.y - aMat[2].z * e.z;

	vertices[5].x = c.x + aMat[0].x * e.x - aMat[1].x * e.y - aMat[2].x * e.z;
	vertices[5].y = c.y + aMat[0].y * e.x - aMat[1].y * e.y - aMat[2].y * e.z;
	vertices[5].z = c.z + aMat[0].z * e.x - aMat[1].z * e.y - aMat[2].z * e.z;

	vertices[6].x = c.x - aMat[0].x * e.x + aMat[1].x * e.y - aMat[2].x * e.z;
	vertices[6].y = c.y - aMat[0].y * e.x + aMat[1].y * e.y - aMat[2].y * e.z;
	vertices[6].z = c.z - aMat[0].z * e.x + aMat[1].z * e.y - aMat[2].z * e.z;

	vertices[7].x = c.x - aMat[0].x * e.x - aMat[1].x * e.y + aMat[2].x * e.z;
	vertices[7].y = c.y - aMat[0].y * e.x - aMat[1].y * e.y + aMat[2].y * e.z;
	vertices[7].z = c.z - aMat[0].z * e.x - aMat[1].z * e.y + aMat[2].z * e.z;

	return vertices;
}
Edge* getEdges(Entity* a) {
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		printf("NOT A BOX!\n");
		return;
	}
	Vector3D *v = getVertices(a);
	int index[][2] = { {6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
						{0,1},{0,3},{7,1},{7,4},{4,5},{5,3}
						};
	for (int j = 0; j < 12; j++) {
		Edge o;
		o.origin = v[index[j][0]];
		o.endpoint = v[index[j][1]];
		edges[j] = o;
	}
	return edges;
}

Plane* getPlanes(Entity* a) {
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		printf("NOT A BOX!\n");
		return;
	}
	Vector3D c = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D e = vector3d(a->rSelf->scale.x, a->rSelf->scale.y, a->rSelf->scale.z);
	Vector3D aMat[3];
	//creating local axis for a
	aMat[0] = vector3d(1, 0, 0);

	aMat[1] = vector3d(0, 1, 0);

	aMat[2] = vector3d(0, 0, 1);

	//rotating them into position
	for (int i = 0; i < 3; i++) {
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));
	}

	planes[0].normal = aMat[0];
	Vector3D dv;
	dv.x = c.x + aMat[0].x * e.x;
	dv.y = c.y + aMat[0].y * e.x;
	dv.z = c.z + aMat[0].z * e.x;
	planes[0].d = vector3d_dot_product(aMat[0], dv);

	planes[1].normal = aMat[0];
	planes[1].normal.x = -planes[1].normal.x;
	planes[1].normal.y = -planes[1].normal.z;
	planes[1].normal.z = -planes[1].normal.z;
	
	dv.x = c.x - aMat[0].x * e.x;
	dv.y = c.y - aMat[0].y * e.x;
	dv.z = c.z - aMat[0].z * e.x;
	planes[1].d = -vector3d_dot_product(aMat[0], dv);

	planes[2].normal = aMat[1];
	
	dv.x = c.x + aMat[1].x * e.y;
	dv.y = c.y + aMat[1].y * e.y;
	dv.z = c.z + aMat[1].z * e.y;
	planes[2].d = vector3d_dot_product(aMat[1], dv);

	planes[3].normal = aMat[1];
	planes[3].normal.x = -planes[3].normal.x;
	planes[3].normal.y = -planes[3].normal.z;
	planes[3].normal.z = -planes[3].normal.z;
	
	dv.x = c.x - aMat[1].x * e.y;
	dv.y = c.y - aMat[1].y * e.y;
	dv.z = c.z - aMat[1].z * e.y;
	planes[3].d = -vector3d_dot_product(aMat[1], dv);

	planes[4].normal = aMat[2];
	
	dv.x = c.x + aMat[2].x * e.z;
	dv.y = c.y + aMat[2].y * e.z;
	dv.z = c.z + aMat[2].z * e.z;
	planes[4].d = vector3d_dot_product(aMat[2], dv);
	
	planes[5].normal = aMat[2];
	planes[5].normal.x = -planes[5].normal.x;
	planes[5].normal.y = -planes[5].normal.z;
	planes[5].normal.z = -planes[5].normal.z;
	
	dv.x = c.x - aMat[2].x * e.z;
	dv.y = c.y - aMat[2].y * e.z;
	dv.z = c.z - aMat[2].z * e.z;
	planes[5].d = -vector3d_dot_product(aMat[2], dv);

	return planes;
}

int clipToPlane(Plane* plane, Edge* line, Vector3D* outPoint) {
	Vector3D ab;
	vector3d_add(ab, line->endpoint, -line->origin);
	float nAb = vector3d_dot_product(plane->normal, ab);
	if (nAb - 0 < 0.0001) {
		return false;
	}
	float nA = vector3d_dot_product(plane->normal, line->origin);
	float t = (plane->d - nA) / nAb;
	if(t >= 0.0 && t <= 1.0){
		if (outPoint != NULL) {
			outPoint->x = line->origin.x + ab.x * t;
			outPoint->y = line->origin.y + ab.y * t;
			outPoint->z = line->origin.z + ab.z * t;
		}
		return true;
	}
	return false;
}
Vector3D* clipEdgesToBox(Edge* edges, Entity* a) {
	Vector3D intersection;
	contSize = 0;
	Plane *planes = getPlanes(a);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 12; j++) {
			if (clipToPlane(&planes[i], &edges[j], &intersection)) {
				Vector3D p = intersection;
				Vector3D tF = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
				vector3d_add(p,p ,-tF);
				Vector3D b = vector3d(a->rSelf->scale.x, a->rSelf->scale.y, a->rSelf->scale.z);
				if (boxSDF(p, b) <= 0) {
					contPoints[i] = intersection;
					contSize++;
				}
			}
		}
	}
	return contPoints;
}

float penetrationDepth(Entity* a, Entity* b, Vector3D axis, int* outShouldFlip) {
	vector3d_normalize(&axis);
	Vector2D i1 = getInterval(a, axis);
	Vector2D i2 = getInterval(b, axis);
	if (!((i2.x <= i1.y) && (i1.x <= i2.y))) {
		return 0;
	}
	float len1 = i1.y - i1.x;
	float len2 = i2.y - i2.x;

	float min = min(i1.x, i2.x);
	float max = max(i1.y, i2.y);

	float length = max - min;
	*outShouldFlip = (i2.x < i1.x);
	return (len1 + len2) - length;
}

/*Signed Distance Functions*/
float sphereSDF(Vector3D p) {
	return length(p) - 0.5; //hard coded to match frag shader
}

float boxSDF(Vector3D p, Vector3D b) {
	Vector3D a = vector3d(abs(p.x), abs(p.y), abs(p.z));
	Vector3D q = vector3d(0, 0, 0);
	vector3d_add(q, a, -b);
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
/*Intersection tests and Collision dection*/
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
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));

		rotX(&bMat[i], radians(-b->rSelf->rotation.x));
		rotY(&bMat[i], radians(-b->rSelf->rotation.y));
		rotZ(&bMat[i], radians(-b->rSelf->rotation.z));

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
		rotX(&n, radians(a->rSelf->rotation.x));
		rotY(&n, radians(a->rSelf->rotation.y));
		rotZ(&n, radians(a->rSelf->rotation.z));
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
		rotX(&n, radians(-a->rSelf->rotation.x));
		rotY(&n, radians(-a->rSelf->rotation.y));
		rotZ(&n, radians(-a->rSelf->rotation.z));
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
		else if (s < 0) {
			a->rSelf->position.y += fabsf(s) + r;
		}
		return;
	}
}
//Physics Functions (Management)
void phyEngine_init() {
	phyEngine.linearProjectionPercent = 0.45;
	phyEngine.penetrationSlack = 0.01;
	phyEngine.impulseIteration = 5;
}
Rigidbody* addRigidbody(Rigidbody body) {
	if (phyEngine.rigidSize + 1 > rigidTotal) {
		printf("THERE IS NO MORE ROOM IN THE RIGIDBODY LIST");
		return NULL;
	}
	int count = phyEngine.rigidSize;
	phyEngine.rigidList[count] = body;
	phyEngine.rigidSize++;
	return &phyEngine.rigidList[count];
}

Obstacle* addObstacle(Obstacle obs) {
	if (phyEngine.obsSize + 1 > obsTotal) {
		printf("THERE IS NO MORE ROOM IN THE OBSTACLE LIST");
		return NULL;
	}
	int count = phyEngine.obsSize;
	phyEngine.obsList[count] = obs;
	phyEngine.obsSize++;
	return &phyEngine.obsList[count];
}
void clearRigidbodys() {
	for (int i = 0; i < rigidTotal; i++) {
		phyEngine.rigidList[i].mass = 0;
	}
	phyEngine.rigidSize = 0;
}

void clearObstacles() {
	for (int i = 0; i < obsTotal; i++) {
		phyEngine.obsList[i].eSelf = NULL;
	}
	phyEngine.rigidSize = 0;
}
PhysicsSystem* get_phyEngine() {
	return &phyEngine;
}
//Collision Manifold functions
void resetCollisionManifold(CollisionManifold* result) {
	if (result != NULL) {
		result->colliding = false;
		result->normal = vector3d(0, 0, 1);
		result->depth = 9999999999999;
		for (int i = 0; i < 24; i++) {
			result->contacts[i] = ZERO3D;
		}
		result->contactSize = 0;
		
	}
}
CollisionManifold findCollisionSpheres(Entity* a, Entity* b) {
	CollisionManifold result;
	resetCollisionManifold(&result);
	if (a->rSelf->type != 0 || a->rSelf->scale.x != a->rSelf->scale.z || a->rSelf->scale.x != a->rSelf->scale.y || a->rSelf->scale.z != a->rSelf->scale.y) {
		printf("A IS NOT A SPHERE!");
		return result;
	}
	if (b->rSelf->type != 0 || b->rSelf->scale.x != b->rSelf->scale.z || b->rSelf->scale.x != b->rSelf->scale.y || b->rSelf->scale.z != b->rSelf->scale.y) {
		printf("B IS NOT A SPHERE!");
		return result;
	}
	

	float r = 0.5 * a->rSelf->scale.x + 0.5 * b->rSelf->scale.x;
	Vector3D d;
	Vector3D aPos = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	Vector3D bPos = vector3d(b->rSelf->position.x, b->rSelf->position.y, b->rSelf->position.z);
	v3_scaler_mult(d, aPos, -1);
	vector3d_add(d, bPos, aPos);
	float magSq = vector3d_magnitude(d) * vector3d_magnitude(d);
	if (magSq - r * r > 0 || magSq == 0) return result;
	vector3d_normalize(&d);
	result.colliding = true;
	result.normal = d;
	result.depth = fabsf(vector3d_magnitude(d) - r) * 0.5;
	float dtp = 0.5 * a->rSelf->scale.x - result.depth;
	Vector3D p; 
	p.x = aPos.x * d.x * dtp;
	p.x = aPos.y * d.y * dtp;
	p.x = aPos.z * d.z * dtp;

	result.contacts[result.contactSize] = p;
	result.contactSize++;

}

CollisionManifold findCollisionSphereBox(Entity* a, Entity* b) {
	CollisionManifold result;
	resetCollisionManifold(&result);
	Vector3D box, sphere, bounds;
	float sphRadius;
	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		sphere = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
		sphRadius = 0.5 * a->rSelf->scale.x;
		box = vector3d(b->rSelf->position.x, b->rSelf->position.y, b->rSelf->position.z);
		bounds = vector3d(b->rSelf->scale.x, b->rSelf->scale.y, b->rSelf->scale.z);
	}
	else if (b->rSelf->type == 0 && b->rSelf->scale.x == b->rSelf->scale.z && b->rSelf->scale.x == b->rSelf->scale.y && b->rSelf->scale.z == b->rSelf->scale.y) {
		sphere = vector3d(b->rSelf->position.x, b->rSelf->position.y, b->rSelf->position.z);
		sphRadius = 0.5 * b->rSelf->scale.x;
		box  = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
		bounds = vector3d(a->rSelf->scale.x, a->rSelf->scale.y, a->rSelf->scale.z);
	}
	else {
		return result;
	}
	Vector3D boxDir,relP;
	vector3d_add(relP, sphere, -box);
	boxDir.x = box.x - sphere.x;
	boxDir.y = box.y - sphere.y;
	boxDir.z = box.z - sphere.z;
	vector3d_normalize(&boxDir);

	Vector3D closeP;
	
	float ctb = boxSDF(relP,bounds);
	if (ctb > sphRadius) return result;
	closeP.x = sphere.x + ctb * boxDir.x;
	closeP.y = sphere.y + ctb * boxDir.y;
	closeP.z = sphere.z + ctb * boxDir.z;

	Vector3D norm;
	if (ctb - 0.0 < 0.001) {
		Vector3D altDir;
		vector3d_add(altDir, closeP, -box);
		float mSq = vector3d_magnitude(altDir);
		if (mSq - 0.0 < 0.001) {
			return result;
		}
		vector3d_normalize(&altDir);
		norm = altDir;
	}
	else {
		Vector3D dir;
		vector3d_add(dir, sphere, -closeP);
		norm = dir;
		vector3d_normalize(&norm);
	}
	Vector3D outsidePoint; 
	outsidePoint.x = sphere.x - norm.x * sphRadius;
	outsidePoint.y = sphere.y - norm.y * sphRadius;
	outsidePoint.z = sphere.z - norm.z * sphRadius;
	Vector3D len;
	vector3d_add(len, closeP, -outsidePoint);
	float dist = vector3d_magnitude(len);
	result.colliding = true;
	Vector3D out;
	out.x = closeP.x + (outsidePoint.x - closeP.x) * 0.5;
	out.y = closeP.y + (outsidePoint.y - closeP.y) * 0.5;
	out.z = closeP.z + (outsidePoint.z- closeP.z) * 0.5;

	result.contacts[result.contactSize] = out;
	result.contactSize++;
	result.normal = norm;
	result.depth = dist * 0.5;

	return result;
}

CollisionManifold findCollisionBoxes(Entity* a, Entity* b) {
	CollisionManifold result;
	resetCollisionManifold(&result);

	Vector3D v[8];
	Vector3D e[12];

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
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));

		rotX(&bMat[i], radians(-b->rSelf->rotation.x));
		rotY(&bMat[i], radians(-b->rSelf->rotation.y));
		rotZ(&bMat[i], radians(-b->rSelf->rotation.z));

	}
	Vector3D test[15] = {
				aMat[0],
				aMat[1],
				aMat[2],
				bMat[0],
				bMat[1],
				bMat[2]
				};
	for (int i = 0; i < 3; i++) { // Fill out rest of axis
		vector3d_cross_product(&test[6 + i * 3 + 0],test[i], test[0]);
		vector3d_cross_product(&test[6 + i * 3 + 1], test[i], test[1]);
		vector3d_cross_product(&test[6 + i * 3 + 2], test[i], test[2]);
	}
	Vector3D hitNormal;
	int isEmp = true;
	int shouldFlip;
	float mag;
	for (int i = 0; i < 15; i++) {
		mag = vector3d_magnitude(test[i]);
		mag *= mag;
		if (mag < 0.001) {
			continue;
		}
		float depth = penetrationDepth(a, b, test[i], &shouldFlip);
		if (depth <= 0) {
			return result;
		}
		else if (depth < result.depth) {
			if (shouldFlip) {
				test[i].x = -test[i].x;
				test[i].y = -test[i].y;
				test[i].z = -test[i].z;
			}
			result.depth = depth;
			hitNormal = test[i];
			isEmp = false;
		}
	}
	if (isEmp) {
		return result;
	}
	int totalSize = 0;
	Vector3D axis = hitNormal;
	vector3d_normalize(&axis);
	Vector3D* cur = clipEdgesToBox(getEdges(b), a);
	Vector3D c1[12];
	totalSize += contSize;
	for (int i = 0; i < contSize; i++) {
		//c1[i] = cur[i];
		result.contacts[i] = cur[i];
	}
	cur = clipEdgesToBox(getEdges(a), b);
	Vector3D c2[12];
	int start = totalSize;
	totalSize += contSize;
	for (int i = start; i < totalSize; i++) {
		//c2[i] = cur[i];
		result.contacts[i] = cur[i];
	}
	result.contactSize = totalSize;
	Vector2D i = getInterval(a, axis); //x- min y - max
	float distance = (i.y - i.x) * 0.5 - result.depth*0.5;
	Vector3D pointOnPlane;
	pointOnPlane.x = a->rSelf->position.x + axis.x*distance;
	pointOnPlane.y = a->rSelf->position.y + axis.x * distance;
	pointOnPlane.z = a->rSelf->position.z + axis.x * distance;

	for (int i = totalSize - 1; i >= 0; i--) {
		Vector3D contact = result.contacts[i];
		Vector3D v;
		vector3d_add(v, pointOnPlane, -contact);
		result.contacts[i].x = contact.x + (axis.x * vector3d_dot_product(axis, v));
		result.contacts[i].y = contact.y + (axis.y * vector3d_dot_product(axis, v));
		result.contacts[i].z = contact.z + (axis.z * vector3d_dot_product(axis, v));
	}

	int startSize = result.contactSize;
	for (int i = startSize - 1; i >= 0; i--) {
		Vector3D contact = result.contacts[i];
		Vector3D v;
		vector3d_add(v, pointOnPlane, -contact);
		result.contacts[i].x = contact.x + (axis.x * vector3d_dot_product(axis, v));
		result.contacts[i].y = contact.y + (axis.y * vector3d_dot_product(axis, v));
		result.contacts[i].z = contact.z + (axis.z * vector3d_dot_product(axis, v));
		for (int j = startSize - 1; j > 0; j--) {
			Vector3D test;
			vector3d_add(test, result.contacts[j], -result.contacts[i]);
			if (vector3d_magnitude(test) < 0.0001) {
				//erases vector by moving it to a very far postion with we will se as a sign of invalidity
				result.contacts[j].x = 999999;
				result.contacts[j].y = 999999;
				result.contacts[j].z = 999999;
				result.contactSize--;
			}
		}
	}


	result.colliding = true;
	result.normal = axis;

	return result;
}
CollisionManifold findCollisionFeatures(Entity* a, Entity* b) {
	CollisionManifold result;
	resetCollisionManifold(&result);

	if (a->rSelf->type == 0 && a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
		if(b->rSelf->type == 0 && b->rSelf->scale.x == b->rSelf->scale.z && b->rSelf->scale.x == b->rSelf->scale.y && b->rSelf->scale.z == b->rSelf->scale.y) {
			result = findCollisionSpheres(a, b);		
		}
		result = findCollisionSphereBox(a,b);
		v3_scaler_mult(result.normal, result.normal, -1);
	}
	else if (b->rSelf->type == 0 && b->rSelf->scale.x == b->rSelf->scale.z && b->rSelf->scale.x == b->rSelf->scale.y && b->rSelf->scale.z == b->rSelf->scale.y) {
		result = findCollisionSphereBox(a, b);

	}
	else {
		result = findCollisionBoxes(a, b);
	}
	return result;
}

void applyImpluse(Entity* a, Entity* b, CollisionManifold* m, int c) {
	// Linear Velocity
	float invMass1 = invMass(a->pSelf);//a->pSelf->invMass(a);
	float invMass2 = invMass(b->pSelf);//b->pSelf->invMass(b);
	float invMassSum = invMass1 + invMass2;
	if (invMassSum == 0.0f) { return; }
	Vector3D r1 ,r2;
	vector3d_add(r1,m->contacts[c], -body_get_pos(a->pSelf));
	vector3d_add(r2, m->contacts[c], -body_get_pos(b->pSelf));
	
	Matrix4 i1;
	gfc_matrix_copy(&i1, invTensor(a->pSelf));
	Matrix4 i2;
	gfc_matrix_copy(&i2, invTensor(b->pSelf));

	Vector3D relVelo;
	Vector3D relB, relA,cross;
	vector3d_cross_product(&cross,b->pSelf->angVel, r2);
	vector3d_add(relB, b->velocity,cross);
	vector3d_cross_product(&cross, a->pSelf->angVel, r1);
	vector3d_add(relA, a->velocity, cross);
	vector3d_add(relVelo, relB, -relA);

	Vector3D relNorm = m->normal;
	vector3d_normalize(&relNorm);
	if (vector3d_dot_product(relVelo, relNorm) > 0) return;

	float e = min(a->pSelf->cor, b->pSelf->cor);
	float num = (-(1 + e)) * vector3d_dot_product(relVelo, relNorm);

	float d1 = invMassSum;
	vector3d_cross_product(&cross, r1,relNorm);
	Vector3D c1 = cross;
	Vector4D in4;
	Vector4D out4;
	in4.x = c1.x;
	in4.y = c1.y;
	in4.z = c1.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i1, in4);
	c1.x = out4.x;
	c1.y = out4.y;
	c1.z = out4.z;
	Vector3D d2;
	vector3d_cross_product(&d2, c1, r1);

	vector3d_cross_product(&cross,  r2,relNorm);
	Vector3D c2 = cross;
	in4.x = c2.x;
	in4.y = c2.y;
	in4.z = c2.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i2, in4);
	c2.x = out4.x;
	c2.y = out4.y;
	c2.z = out4.z;
	Vector3D d3;
	vector3d_cross_product(&d3, c2, r2);

	Vector3D dTotal;
	vector3d_add(dTotal, d2, d3);
	float den = d1 + vector3d_dot_product(relNorm, dTotal);


	float j = (den == 0.0f) ? 0.0f :
		num / den;
	if (m->contactSize > 0 && j != 0) {
		j /= (float)m->contactSize;
	}
	Vector3D impluse = vector3d(relNorm.x * j, relNorm.y * j, relNorm.z * j);
	vector3d_add(a->velocity, a->velocity, -invMass1*impluse);
	vector3d_add(b->velocity, b->velocity, invMass2 * impluse);
	//Angler stuff - a
	vector3d_cross_product(&cross, r1,impluse);
	Vector3D tmp = cross;
	in4.x = tmp.x;
	in4.y = tmp.y;
	in4.z = tmp.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i1, in4);
	tmp.x = out4.x;
	tmp.y = out4.y;
	tmp.z = out4.z;
	vector3d_add(a->pSelf->angVel, a->pSelf->angVel, -tmp);
	//b
	vector3d_cross_product(&cross, r2,impluse);
	tmp = cross;
	in4.x = tmp.x;
	in4.y = tmp.y;
	in4.z = tmp.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i2, in4);
	tmp.x = out4.x;
	tmp.y = out4.y;
	tmp.z = out4.z;
	vector3d_add(b->pSelf->angVel, b->pSelf->angVel, tmp);


	//Friction
	Vector3D t;
	vector3d_add(t, relVelo, -vector3d_dot_product(relNorm, relVelo) * relNorm);
	if (vector3d_magnitude(t) - 0 < 0.0000000001) {
		return;
	}
	vector3d_normalize(&t);
	num = -vector3d_dot_product(relVelo, t);

	d1 = invMassSum;
	vector3d_cross_product(&cross, r1,t);
	c1 = cross;
	in4.x = c1.x;
	in4.y = c1.y;
	in4.z = c1.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i1, in4);
	c1.x = out4.x;
	c1.y = out4.y;
	c1.z = out4.z;
	vector3d_cross_product(&d2, c1, r1);

	vector3d_cross_product(&cross, r2,t);
	c2 = cross;
	in4.x = c2.x;
	in4.y = c2.y;
	in4.z = c2.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i2, in4);
	c2.x = out4.x;
	c2.y = out4.y;
	c2.z = out4.z;
	vector3d_cross_product(&d3, c2, r2);
	vector3d_add(dTotal, d2, d3);
	den = d1 + vector3d_dot_product(t, dTotal);
	if (den == 0) return;

	float jt = num / den;
	if (m->contactSize > 0 && jt != 0) {
		jt /= (float)m->contactSize;
	}
	if (fabsf(jt) - 0.0 < 0.0000000001) {
		return;
	}
	float friction = sqrtf(a->pSelf->friction * b->pSelf->friction);
	if (jt > j * friction) {
		jt = j * friction;
	}
	else if (jt < -j * friction) {
		jt = -j * friction;
	}
	Vector3D tangentImpluse;
	tangentImpluse.x = t.x * jt;
	tangentImpluse.y = t.y * jt;
	tangentImpluse.z = t.z * jt;

	vector3d_add(a->velocity, a->velocity, -invMass1 * tangentImpluse);
	vector3d_add(b->velocity, b->velocity, invMass2 * tangentImpluse);
	
	vector3d_cross_product(&cross, r1,tangentImpluse);
	tmp = cross;
	in4.x = tmp.x;
	in4.y = tmp.y;
	in4.z = tmp.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i1, in4);
	tmp.x = out4.x;
	tmp.y = out4.y;
	tmp.z = out4.z;
	vector3d_add(a->pSelf->angVel, a->pSelf->angVel, -tmp);
	//b
	vector3d_cross_product(&cross,r2, tangentImpluse);
	tmp = cross;
	in4.x = tmp.x;
	in4.y = tmp.y;
	in4.z = tmp.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, i2, in4);
	tmp.x = out4.x;
	tmp.y = out4.y;
	tmp.z = out4.z;
	vector3d_add(b->pSelf->angVel, b->pSelf->angVel, tmp);
}

//Physics Functions (Torque and Force functions)
void physicsUpdate(float deltatime) {
	SDL_memset(phyEngine.colliders1, 0, sizeof(phyEngine.colliders1));
	SDL_memset(phyEngine.colliders2, 0, sizeof(phyEngine.colliders2));
	for (int i = 0; i < 100; i++) {
		resetCollisionManifold(&phyEngine.results[i]);
	}
	phyEngine.pairSize = 0;
	int size = phyEngine.rigidSize;
	for (int i = 0; i < size; i++) {
		for (int j = i; j < size; j++) {
			if (i == j) continue;
			CollisionManifold result;
			resetCollisionManifold(&result);
			result = findCollisionFeatures(phyEngine.rigidList[i].eSelf, phyEngine.rigidList[j].eSelf);
			int noCollide = (phyEngine.rigidList[i].eSelf->noCollide || phyEngine.rigidList[j].eSelf->noCollide);
			if (result.colliding) {
				phyEngine.rigidList[i].eSelf->touch(phyEngine.rigidList[i].eSelf, phyEngine.rigidList[j].eSelf);
				phyEngine.rigidList[j].eSelf->touch(phyEngine.rigidList[j].eSelf, phyEngine.rigidList[i].eSelf);
				if (!noCollide) {
					phyEngine.colliders1[i] = phyEngine.rigidList[i];
					phyEngine.colliders2[i] = phyEngine.rigidList[j];
					phyEngine.results[i] = result;
					phyEngine.pairSize++;
				}
			}
		}
		
	}
	for (int i = 0; i < size; i++) {
		phyEngine.rigidList[i].applyForces(&phyEngine.rigidList[i]);
	}
	//Impluses made from forces
	for (int k = 0; k < phyEngine.impulseIteration; k++) {
		int i = 0;
		int count = 0;
		while (count < phyEngine.pairSize) {
			if (phyEngine.results[i].colliding == false) {
				i++;
				continue;
			}
			int jSize = phyEngine.results[i].contactSize;
			int j = 0;
			int rCount = 0;
			while (rCount < jSize) {
				if (phyEngine.results[i].contacts[j].x == 9999999) {
					j++;
					continue;
				}
				Entity* a = phyEngine.colliders1[i].eSelf;
				Entity* b = phyEngine.colliders2[i].eSelf;
				applyImpluse(a, b,&phyEngine.results[i], j);
				j++;
				rCount++;
			}
			i++;
			count++;
		}
	}
	for (int i = 0; i < size; i++) {
		phyEngine.rigidList[i].update(&phyEngine.rigidList[i],deltatime);
	}
	size = phyEngine.pairSize;
	int count = 0;
	int in = 0;
	while (count < size ){
		if (phyEngine.colliders1[in].eSelf == NULL && phyEngine.colliders2[in].eSelf == NULL) {
			in++;
			continue;
		}
		float totalMass = invMass(&phyEngine.colliders1[in]) + invMass(&phyEngine.colliders2[in]);
		if (totalMass == 0) {
			in++;
			count++;
			continue;
		}
		float depth = fmaxf(phyEngine.results[in].depth - phyEngine.penetrationSlack, 0);
		float scalar = depth / totalMass;
		Vector3D correction = phyEngine.results[in].normal;
		v3_scaler_mult(correction, correction, scalar*phyEngine.linearProjectionPercent);
		
		phyEngine.colliders1[in].eSelf->rSelf->position.x += -correction.x * invMass(&phyEngine.colliders1[in]);
		phyEngine.colliders1[in].eSelf->rSelf->position.y += -correction.y * invMass(&phyEngine.colliders1[in]);
		phyEngine.colliders1[in].eSelf->rSelf->position.z += -correction.z * invMass(&phyEngine.colliders1[in]);
		
		phyEngine.colliders2[in].eSelf->rSelf->position.x += -correction.x * invMass(&phyEngine.colliders2[in]);
		phyEngine.colliders2[in].eSelf->rSelf->position.y += -correction.y * invMass(&phyEngine.colliders2[in]);
		phyEngine.colliders2[in].eSelf->rSelf->position.z += -correction.z * invMass(&phyEngine.colliders2[in]);
		in++;
		count++;
	}
	size = phyEngine.rigidSize;
	for (int i = 0; i < size; i++) {
		phyEngine.rigidList[i].solveObstacles(&phyEngine.rigidList[i],phyEngine.obsList);
		//groundCheck(phyEngine.rigidList[i].eSelf);
	}
}

void body_ApplyForces(Rigidbody *self) {
	if (self->eSelf == NULL || self->eSelf->inuse == 0) return;
	self->forces = self->gravity;
	v3_scaler_mult(self->forces, self->forces, self->mass);
	
}
Vector3D body_get_pos(Rigidbody* self) {
	if (self->eSelf == NULL || self->eSelf->inuse == 0) return;
	float x = self->eSelf->rSelf->position.x;
	float y = self->eSelf->rSelf->position.y;
	float z = self->eSelf->rSelf->position.z;
	return vector3d(x, y, z);
}
//Should be called at a fixed fps! [possibly delete?]
void body_update(Rigidbody* self, float deltatime) {
	if (self->eSelf == NULL || self->eSelf->inuse == 0) return;
	Vector3D velocity;
	v3_scaler_mult(velocity, self->oldPosition, -1);
	vector3d_add(velocity, velocity, body_get_pos(self));
	self->oldPosition = body_get_pos(self);
	float deltaSquare = deltatime * deltatime;

	float vx = velocity.x * self->friction + self->forces.x * deltaSquare;
	float vy = velocity.y * self->friction + self->forces.y * deltaSquare;
	float vz = velocity.z * self->friction + self->forces.z * deltaSquare;
	Vector3D out = vector3d(vx,vy,vz);
	vector3d_add(self->eSelf->rSelf->position, self->eSelf->rSelf->position, out);
}

void body_solveObstacles(Rigidbody* self, Obstacle* obsList) {
	if (self->eSelf == NULL || self->eSelf->inuse == 0) return;
	int size = phyEngine.obsSize;
	int count = 0;
	int i = 0;
	while(count < size) {
		if (obsList[i].eSelf == NULL) {
			i++;
			continue;
		}
		Vector3D pos = vector3d(self->eSelf->rSelf->position.x, self->eSelf->rSelf->position.y, self->eSelf->rSelf->position.z);
		
		if (linetest(obsList[i].eSelf, self->oldPosition, pos)) {
			obsList[i].eSelf->touch(obsList[i].eSelf, self->eSelf);
			self->eSelf->touch(self->eSelf, obsList[i].eSelf);
			if (obsList[i].eSelf->noCollide || self->eSelf->noCollide) continue;
			Vector3D velocity;
			v3_scaler_mult(velocity, self->oldPosition, -1);
			vector3d_add(velocity, velocity, pos);
			Vector3D dir = velocity;
			vector3d_normalize(&dir);
			Ray ray;
			ray.origin = self->oldPosition;
			ray.direction = dir;
			RaycastResult result;
			if (raycastGeneral(obsList[i].eSelf, &ray, &result)) {
				self->eSelf->rSelf->position.x = result.point.x + result.normal.x * 0.003;
				self->eSelf->rSelf->position.y = result.point.y + result.normal.y * 0.003;
				self->eSelf->rSelf->position.z = result.point.z + result.normal.z * 0.003;
				
				Vector3D vn, vt;
				vn.x = result.normal.x * vector3d_dot_product(result.normal, velocity);
				vn.y = result.normal.y * vector3d_dot_product(result.normal, velocity);
				vn.z = result.normal.z * vector3d_dot_product(result.normal, velocity);
				
				vt.x = velocity.x - vn.x;
				vt.y = velocity.y - vn.y;
				vt.z = velocity.z - vn.z;

				//self->oldPosition = pos;

				self->oldPosition.x = pos.x + (vt.x - vn.x * self->bounce);
				self->oldPosition.y = pos.y + (vt.y - vn.y * self->bounce);
				self->oldPosition.z = pos.z + (vt.z - vn.z * self->bounce);
				
			}
			
		}
		i++;
		count++;
	}
}

Matrix4* invTensor(Rigidbody* self) {
	float ix = 0.0f;
	float iy = 0.0f;
	float iz = 0.0f;
	float iw = 0.0f;
	if (self->mass != 0) {
		if (self->eSelf->rSelf->type == 0 && self->eSelf->rSelf->scale.x == self->eSelf->rSelf->scale.z && self->eSelf->rSelf->scale.x == self->eSelf->rSelf->scale.y && self->eSelf->rSelf->scale.z == self->eSelf->rSelf->scale.y) {
			float r2 = self->eSelf->rSelf->scale.x;
			r2 *= r2;
			float fraction = (2 / 5);
			ix = r2 * self->mass * fraction;
			iy = r2 * self->mass * fraction;
			iz = r2 * self->mass * fraction;
			iw = 1.0f;
		}
		else if (self->eSelf->rSelf->type == 1) {
			Vector3D size = vector3d(self->eSelf->rSelf->scale.x, self->eSelf->rSelf->scale.y, self->eSelf->rSelf->scale.z);
			float fraction = (1.0f / 12.0f);
			float x2 = size.x * size.x;
			float y2 = size.y * size.y;
			float z2 = size.z * size.z;
			ix = (y2 + z2) * self->mass * fraction;
			iy = (x2 + z2) * self->mass * fraction;
			iz = (x2 + y2) * self->mass * fraction;
			iw = 1.0f;

		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (j == i) {
				if (j == 0) tensor[i][j] = ix;
				if (j == 1) tensor[i][j] = iy;
				if (j == 2) tensor[i][j] = iz;
				if (j == 3) tensor[i][j] = iw;
			}
			else tensor[i][j] = 0;
		}
	}
	return tensor;
}

void body_addRotationalImpulse(Rigidbody* self,Vector3D point, Vector3D impulse) {
	Vector3D centerOfMass = vector3d(self->eSelf->rSelf->position.x, self->eSelf->rSelf->position.y, self->eSelf->rSelf->position.z);
	Vector3D torque;
	Vector3D v1;
	vector3d_add(v1, point, -centerOfMass);
	vector3d_cross_product(&torque, v1, impulse);

	Vector4D out4;
	Vector4D in4;
	in4.x = torque.x;
	in4.y = torque.y;
	in4.z = torque.z;
	in4.w = 0;
	gfc_matrix_multiply_vector4d(&out4, invTensor(self), in4);
	Vector3D angAcc;
	angAcc.x = out4.x;
	angAcc.y = out4.y;
	angAcc.z = out4.z;
	vector3d_add(self->angVel, self->angVel, angAcc);
}

