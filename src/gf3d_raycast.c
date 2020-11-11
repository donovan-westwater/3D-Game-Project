#include "gf3d_raycast.h"
#include "gf3d_physics.h"



void resetRayResult(RaycastResult* outResult) {
	if (outResult != NULL) {
		outResult->t = -1;
		outResult->hit = false;
		outResult->normal = vector3d(0, 0, 1);
		outResult->point = vector3d(0, 0, 0);
	}
}

//Assume a is a sphere -- returns t which is time of collision
int raycastSphere(Entity* a, Ray* ray, RaycastResult* outResult) {
	resetRayResult(outResult);
	//Validates sphere
	if (a->rSelf->type != 0 || a->rSelf->scale.x != a->rSelf->scale.z || a->rSelf->scale.x != a->rSelf->scale.y || a->rSelf->scale.z != a->rSelf->scale.y) return -1;
	Vector3D e;
	Vector3D spherePos = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	
	vector3d_add(e, spherePos, -ray->origin);
	float rSq = a->rSelf->scale.x * a->rSelf->scale.x * 0.5 * 0.5;
	float eSq = vector3d_magnitude(e);
	float t;
	eSq = eSq * eSq;
	//Normalize ray direction (It should be normalized)
	vector3d_normalize(&ray->direction);
	float aDot = vector3d_dot_product(e, ray->direction);

	float bSq = eSq - (aDot * aDot);
	float f = sqrtf(rSq - bSq);
	t = aDot - f;

	if (rSq - (eSq - (aDot * aDot)) < 0.0) return false;
	else if (eSq < rSq) return t = aDot + f;

	if (outResult != NULL) {
		outResult->t = t;
		outResult->hit = true; 
		Vector3D rt = vector3d(ray->direction.x * t, ray->direction.y * t, ray->direction.z * t);
		vector3d_add(outResult->point, ray->origin, rt);
		vector3d_add(outResult->normal, outResult->point, -a->rSelf->position);
		vector3d_normalize(&outResult->normal);
	}


	return true;
}



//Assume a is a Box -- returns t which is time of collision
int raycastBox(Entity* a, Ray* ray, RaycastResult* outResult){
	resetRayResult(outResult);
	if (a->rSelf->type == 0) return -1;
	Vector3D aMat[3];
	float bounds[3];
	bounds[0] = 0.25 * a->rSelf->scale.x;
	bounds[1] = 0.25 * a->rSelf->scale.y;
	bounds[2] = 0.25 * a->rSelf->scale.z;
	//creating local axis for a and b
	aMat[0] = vector3d(1, 0, 0); //X
	
	aMat[1] = vector3d(0, 1, 0); //Y
	
	aMat[2] = vector3d(0, 0, 1); //Z

	//rotating them into position
	for (int i = 0; i < 3; i++) {
		rotX(&aMat[i], radians(-a->rSelf->rotation.x));
		rotY(&aMat[i], radians(-a->rSelf->rotation.y));
		rotZ(&aMat[i], radians(-a->rSelf->rotation.z));
	}
	Vector3D p;
	Vector3D oPos = vector3d(a->rSelf->position.x, a->rSelf->position.y, a->rSelf->position.z);
	vector3d_add(p, oPos, -ray->origin);
	vector3d_normalize(&ray->direction);
	Vector3D f;
	Vector3D e;
	f.x = vector3d_dot_product(aMat[0], ray->direction);
	f.y = vector3d_dot_product(aMat[1], ray->direction);
	f.z = vector3d_dot_product(aMat[2], ray->direction);

	e.x = vector3d_dot_product(aMat[0], p);
	e.y = vector3d_dot_product(aMat[1], p);
	e.z = vector3d_dot_product(aMat[2], p);

	float t[6] = { 0,0,0,0,0,0 };
	//X - axis
	if (fabsf(f.x - 0.0) < 0.0001) {
		if (-e.x - bounds[0] > 0 || -e.x + bounds[0] < 0) return -1;
		f.x = 0.00001;
	}
	t[0] = (e.x + bounds[0]) / f.x;
	t[1] = (e.x - bounds[0]) / f.x;
	//Y - axis
	if (fabsf(f.y - 0.0) < 0.0001) {
		if (-e.y - bounds[0] > 0 || -e.y + bounds[0] < 0) return -1;
		f.y = 0.00001;
	}
	t[2] = (e.y + bounds[1]) / f.y;
	t[3] = (e.y - bounds[1]) / f.y;
	//Z - axis
	if (fabsf(f.z - 0.0) < 0.0001) {
		if (-e.z - bounds[0] > 0 || -e.z + bounds[0] < 0) return -1;
		f.z = 0.00001;
	}
	t[4] = (e.z + bounds[1]) / f.z;
	t[5] = (e.z - bounds[1]) / f.z;

	float tmin = fmaxf(
		fmaxf(
			fminf(t[0], t[1]),
			fminf(t[2], t[3])),
		fminf(t[4], t[5])
	);
	float tmax = fminf(
		fminf(
			fmaxf(t[0], t[1]),
			fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5])
	);
	if (tmax < 0) return false;
	if (tmin > tmax) return false;
	float result = tmin;
	if (tmin < 0) result = tmax;

	if (outResult != NULL) {
		outResult->t = result;
		outResult->hit = true;
		Vector3D rt = vector3d(ray->direction.x * result, ray->direction.y * result, ray->direction.z * result);
		vector3d_add(outResult->point, ray->origin, rt);
		Vector3D norms[] = {
			aMat[0],vector3d(-1.0 * aMat[0].x,-1.0 * aMat[0].y ,-1.0 * aMat[0].z),
			aMat[1],vector3d(-1.0 * aMat[1].x,-1.0 * aMat[1].y ,-1.0 * aMat[1].z),
			aMat[2],vector3d(-1.0 * aMat[2].x,-1.0 * aMat[2].y ,-1.0 * aMat[2].z)
		};
		for (int i = 0; i < 6; i++) {
			if (result - t[i] < 0.00000001) {
				outResult->normal = norms[i];
				vector3d_normalize(&outResult->normal);
			}
		}
		
	}

	return true;

}
int raycastPlane(Vector3D n, float d, Ray* ray, RaycastResult* outResult) {
	resetRayResult(outResult);
	vector3d_normalize(&n);
	float nd = vector3d_dot_product(ray->direction, n);
	float pn = vector3d_dot_product(ray->origin, n);

	if (nd >= 0.0f) {
		return false;
	}
	float t = (d - pn) / nd;
	if (t >= 0.0f) {
		if (outResult != NULL) {
			outResult->t = t;
			outResult->hit = true;
			Vector3D rt = vector3d(ray->direction.x * t, ray->direction.y * t, ray->direction.z * t);
			vector3d_add(outResult->point, ray->origin, rt);
			outResult->normal = n;
			vector3d_normalize(&outResult->normal);
		}
		return true;
	}
	return false;
}



int raycastGeneral(Entity* a,Ray *ray, RaycastResult* outResult) {
	int type = a->rSelf->type;
	if (type == 0) {
		if (a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
			type = 0;
		}
		else {
			type = 1;
		}
	}
	int bol = false;
	;
	switch (type) {
	case 0:
		bol = raycastSphere(a, ray, outResult);
		break;
	case 1:
		bol = raycastBox(a, ray, outResult);
		break;
	default:
		bol = raycastSphere(a, ray, outResult);
		break;
	}
	return bol;
}



int linetest(Entity* a, Vector3D posA, Vector3D posB) {
	int type = a->rSelf->type;
	if (type == 0) {
		if (a->rSelf->scale.x == a->rSelf->scale.z && a->rSelf->scale.x == a->rSelf->scale.y && a->rSelf->scale.z == a->rSelf->scale.y) {
			type = 0;
		}
		else {
			type = 1;
		}
	}
	int bol = false;
	Ray ray;
	ray.origin = posA;
	Vector3D s = vector3d(-posA.x, -posA.y, -posA.z);
	vector3d_add(ray.direction, posB, s);
	vector3d_normalize(&ray.direction);
	RaycastResult result;
	switch (type) {
	case 0:
		bol = raycastSphere(a,&ray,&result);
		break;
	case 1:
		bol = raycastBox(a, &ray, &result);
		break;
	default:
		bol = raycastSphere(a, &ray, &result);
		break;
	}
	if (bol) {
		float t = result.t;
		Vector3D line;
		vector3d_add(line, posB, posA);
		return t >= 0 && t <= vector3d_magnitude(line);
	}
	return bol;
}