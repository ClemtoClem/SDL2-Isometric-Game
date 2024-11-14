#ifndef __COMPONENT_VELOCITY_H
#define __COMPONENT_VELOCITY_H

#include <SDL2/SDL.h>

//forward declaration of Scene, allows us to use the Scene without causing a cross-referencing header error
typedef struct Scene Scene;

typedef struct ComponentVelocity {
    float x;            //x velocity
    float y;            //y velocity
    int maxVelocity;    //max velocity
    float friction;     //friction for the velocity
} ComponentVelocity;

ComponentVelocity *ComponentVelocity_New();
void ComponentVelocity_AllocateMoreMemory(Scene *scene,int componentIndex);
void ComponentVelocity_Free(ComponentVelocity *positionVelocity);
void ComponentVelocity_SetMaxVelocity(ComponentVelocity *velocityComponent,Uint32 entity,int maxVelocity);
void ComponentVelocity_SetFriction(ComponentVelocity *velocityComponent,Uint32 entity,float friction);
void ComponentVelocity_SetVelocity(ComponentVelocity *velocityComponents,Uint32 entity,float x,float y);

#endif // __COMPONENT_VELOCITY_H