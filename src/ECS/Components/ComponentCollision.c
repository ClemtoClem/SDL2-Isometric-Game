#include <stdio.h>
#include "ComponentCollision.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentCollision *ComponentCollision_New() {
    Uint32 i = 0;    //allocate memory for new collision components
    ComponentCollision *newCollisionComponent = malloc(sizeof(struct ComponentCollision)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if (newCollisionComponent == NULL) {
        WriteError("Could not allocate memory for collision components");
        return NULL;
    }
    //loop through all entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        //set collision type to deactivated
        newCollisionComponent[i].collisionType = COLLISIONTYPE_DEACTIVATED;
        //create a default collision rectangle
        SetupRect(&newCollisionComponent[i].rect,0,0,5,5);
        //set the is colliding flag to 0
        newCollisionComponent[i].isColliding = 0;
    }
    //return the pointer to the collision components
    return newCollisionComponent;
}

void ComponentCollision_AllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentCollision *newComponentCollision = realloc((ComponentCollision*)scene->components[componentIndex].data,sizeof(struct ComponentCollision)*scene->maxEntities);
    if (newComponentCollision == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for collision components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentCollision[j].collisionType = COLLISIONTYPE_DEACTIVATED;
        SetupRect(&newComponentCollision[j].rect,0,0,5,5);
        newComponentCollision[j].isColliding = 0;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentCollision;
}

void ComponentCollision_SetCollisionType(ComponentCollision *collisionComponent,Uint32 entity,CollisionType collisionType) {
    if (collisionComponent != NULL) {
        collisionComponent[entity].collisionType = collisionType;
    }
    else {
        WriteError("Parameter 'ComponentCollision *collisionComponent' is NULL");
    }
}

void ComponentCollision_SetCollisionRectangle(ComponentCollision *collisionComponent,Uint32 entity,SDL_Rect *collisionRect) {
    //make sure that the pointers are not NULL
    if (collisionComponent == NULL) {
        WriteError("Parameter 'ComponentCollision *collisionComponent' is NULL");
        return;
    }
    else if (collisionRect == NULL) {
        WriteError("Parameter 'SDL_Rect *collisionRect' is NULL");
        return;
    }
    //dereference the collision rectangle pointer and set the collision rectangle value
    collisionComponent[entity].rect = *collisionRect;
}

void ComponentCollision_Collision(ComponentCollision *collisionComponent) {
    if (collisionComponent!=NULL) {
        free(collisionComponent);
    }
}
