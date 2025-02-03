#include <stdlib.h>
#include "ComponentVelocity.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentVelocity *ComponentVelocity_New() {
    Uint32 i = 0;

    //allocate memory for new velocity components
    ComponentVelocity *newVelComponent = malloc(sizeof(struct ComponentVelocity)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if (newVelComponent==NULL) {
        //write the error to the logfile
        WriteError("Could not allocate memory for velocity components! ");
        return NULL;
    }

    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        //initialize variables
        newVelComponent[i].x = 0;
        newVelComponent[i].y = 0;
        newVelComponent[i].maxVelocity = 1000;
        newVelComponent[i].friction = 1;
    }

    //return the pointer to the velocity components
    return newVelComponent;
}

void ComponentVelocity_AllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentVelocity *newComponentVelocity = realloc((ComponentVelocity*)scene->components[componentIndex].data,sizeof(struct ComponentVelocity)*scene->maxEntities);
    if (newComponentVelocity == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for velocity components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentVelocity[j].x = 0;
        newComponentVelocity[j].y = 0;
        newComponentVelocity[j].friction = 1;
        newComponentVelocity[j].maxVelocity = 1000;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentVelocity;
}
void ComponentVelocity_Free(ComponentVelocity *velocityComponent) {
    //if the velocity component is not NULL
    if (velocityComponent!=NULL) {
        //free it
        free(velocityComponent);
    }
}

void ComponentVelocity_SetVelocity(ComponentVelocity *velocityComponents,Uint32 entity,float x,float y) {
    if (velocityComponents != NULL) {
        velocityComponents[entity].x =  x;
        velocityComponents[entity].y =  y;
    }
}

void ComponentVelocity_SetMaxVelocity(ComponentVelocity *velocityComponent,Uint32 entity,int maxVelocity) {
    //if the velocity component is not NULL
    if (velocityComponent==NULL) {
        //write the error to the logfile
        WriteError("Parameter:'ComponentVelocity *velocityComponent' is NULL.");
        return;
    }

    if (maxVelocity < 0) {
        //write the error to the logfile
        WriteError("Parameter:'int maxVelocity' cannot have a negative value. Aborting.");
        return;
    }
    velocityComponent[entity].maxVelocity = maxVelocity;

}
void ComponentVelocity_SetFriction(ComponentVelocity *velocityComponent,Uint32 entity,float friction) {
    //if the velocity component is not NULL
    if (velocityComponent == NULL) {
        //write the error to the logfile
        WriteError("Parameter:'ComponentVelocity *velocityComponent' is NULL.");
        return;
    }

    if (friction < 0) {
        //write the error to the logfile
        WriteError("Parameter:'float friction' cannot have a negative value. Aborting.");
        return;
    }
    velocityComponent[entity].friction = friction;
}
