#ifndef __COMPONENT_POSITION_H
#define __COMPONENT_POSITION_H

#include <SDL2/SDL.h>

#define NUM_OF_POSITION_HISTORY 6

//forward declaration of Scene, allows us to use the Scene without causing a cross-referencing header error
typedef struct Scene Scene;

typedef struct ComponentPosition {
    float x;        //x position
    float y;        //y position
    float oldx[NUM_OF_POSITION_HISTORY];  //old x position history frames
    float oldy[NUM_OF_POSITION_HISTORY];  //old y position history frames
    float xOffset;  //x base, used to define where on a object its base is
    float yOffset;  //y base, used to define where on a object its base is
} ComponentPosition;

ComponentPosition *ComponentPosition_New();
void ComponentPosition_Free(ComponentPosition *positionComponent);
void ComponentPosition_AllocateMoreMemory(Scene *scene,int componentIndex);
void ComponentPosition_SetOffset(ComponentPosition *positionComponent,Uint32 entity,float x,float y);
void ComponentPosition_SetPosition(ComponentPosition *positionComponent,Uint32 entity,float x,float y);
void ComponentPosition_AddOldPositionToStack(ComponentPosition *positionComponent, Uint32 entity);
#endif // __COMPONENT_POSITION_H
