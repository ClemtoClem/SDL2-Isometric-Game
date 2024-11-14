#ifndef __COMPONENT_COLLISION_H
#define __COMPONENT_COLLISION_H

#include <SDL2/SDL.h>
#include "../../IsoEngine/isoEngine.h"

//forward declaration of Scene, allows us to use the Scene struct without causing a cross-referencing header error
typedef struct Scene Scene;

typedef enum CollisionType {
    COLLISIONTYPE_DEACTIVATED       = 0,
    COLLISIONTYPE_WORLD             = 1,
    COLLISIONTYPE_ENTITY            = 2,
    COLLISIONTYPE_WORLD_AND_ENTITY  = 3,
} CollisionType;

typedef struct ComponentCollision {
    CollisionType collisionType;   //which collisions to apply to the entity
    SDL_Rect rect;                  //collision rectangle
    SDL_Rect worldRect;             //collision rectangle in world coordinates
    short isColliding;              //flag to mark that there was a collision
} ComponentCollision;

ComponentCollision *ComponentCollision_New();
void ComponentCollision_AllocateMoreMemory(Scene *scene,int componentIndex);
void ComponentCollision_Collision(ComponentCollision *collisionComponent);
void ComponentCollision_SetCollisionType(ComponentCollision *collisionComponent, Uint32 entity, CollisionType collisionType);
void ComponentCollision_SetCollisionRectangle(ComponentCollision *collisionComponent, Uint32 entity, SDL_Rect *collisionRect);
#endif // __COMPONENT_COLLISION_H
