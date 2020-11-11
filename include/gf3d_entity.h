#ifndef __GF3D_ENTITY_H__
#define __GF3D_ENTITY_H__
#include "gfc_vector.h"

#define entSize 50
typedef struct EntityRender_S { //Stores the infomation needed by the shader
    Vector4D position;
    Vector4D rotation;
    Vector4D scale;
    Vector4D color;
    __declspec(align(16)) int id;
    int type;
}EntityRender; 

struct Entity_S;

typedef struct Obstacle_S {
    struct Entity_S* eSelf;
}Obstacle;

//Put inside entity as a pointer!
typedef struct Rigidbody_S {
    struct Entity_S* eSelf;

    //Will likely be removed
    Vector3D gravity; 
    float friction; 
    //Particle values
    float mass; 
    float bounce;
    float cor; //coeffiencet of resitution
    Vector3D oldPosition;
    Vector3D forces;
    //Angular forces
    Vector3D orient;
    Vector3D angVel;
    Vector3D torques; //Sum of torgques


    //Use eSelf->rSelf->pos for current pos
    //use eSelf->velocity for velocity (move over into rigid later)
    void(*update)(struct Rigidbody_S* self, float time);
    void(*applyForces)(struct Rigidbody_S* self);
    void(*solveObstacles)(struct Rigidbody_S* self,Obstacle *obsList);
    float(*invMass)(struct Rigidbody_S* self);
    void(*addLinearImpluse)(struct Rigidbody_S* self, Vector3D impluse);
    void(*addRotationalImpluse)(struct Rigidbody_S* self, Vector3D point, Vector3D impluse);
}Rigidbody;

//no need for a draw funtion on this since its constantly being rendered at all times.
//rembember to set BOTH entity and EntityRender ids
 typedef struct Entity_S
{
     int id;
     int inuse;
     //int type; //sphere by default;
     EntityRender *rSelf; //Pass this to the uniform to draw changes!
     Rigidbody* pSelf;
     Obstacle* obsSelf;
     int isObs;
     int noCollide;
     float timer;
     Vector3D velocity;
     //Possible change entityRender into a pointer to its proper render in the list;
     //Functions (Implement later)
     void(*update)(struct Entity_S* self);
     void(*destory)(struct Entity_S* self);
     void(*touch)(struct Entity_S* self, struct Entity_S* other);
     void(*think)(struct Entity_S* self);
}Entity;

 void initEntList();

 Entity* addEntity(Vector4D pos, Vector4D rot, Vector4D scale, Vector4D color, Vector3D velo,int type,int isObs);

 void destroy(Entity* self);
 
 void entity_touch(Entity* self, Entity* other);

 void updateEntAll();

 void update(Entity *self);

 void rigidbody_update(Rigidbody* self, float time);

 void rigidbody_applyforces(Rigidbody* self);

 float invMass(Rigidbody* self);

 void addLinearImpluse(Rigidbody* self,Vector3D impluse);

 Entity* addCollctible(Vector3D pos);

 Entity* getEntList();

 Entity* addEmpty(Vector3D pos);

 void empty_update(Entity* self);

 void addWalls();

 void coll_update(Entity* self);

#endif