#ifndef __GF3D_ENTITY_H__
#define __GF3D_ENTITY_H__
#include "gfc_vector.h"
#pragma pack(push,32)
typedef struct EntityRender_S { //Stores the infomation needed by the shader
    Vector4D position;
    Vector4D rotation;
    Vector4D scale;
    Vector4D color;
    __declspec(align(16)) int id;
}EntityRender;
#pragma pack(pop)
//no need for a draw funtion on this since its constantly being rendered at all times.
//rembember to set BOTH entity and EntityRender ids
 typedef struct Entity_S
{
     int id;
     int inuse;
     int type; //sphere by default;
     EntityRender *rSelf; //Pass this to the uniform to draw changes!
     //Possible change entityRender into a pointer to its proper render in the list;
     //Functions (Implement later)
     void(*update)(struct Entity_S* self);
     void(*destory)(struct Entity_S* self);
     void(*touch)(struct Entity_S* self, struct Entity_S* other);
     void(*think)(struct Entity_S* self);
}Entity;

 void initEntList();

 void addEntity(Vector4D pos, Vector4D rot, Vector4D scale, Vector4D color, int type);

 void destroy(Entity* self);
 
 void updateEntAll();

 void update(Entity *self);


#endif