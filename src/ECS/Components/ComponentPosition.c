#include <stdlib.h>
#include "ComponentPosition.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentPosition *ComponentPosition_New() {
    Uint32 i = 0, j = 0;
    //allocate memory for new position components
    ComponentPosition *newPosComponent = malloc(sizeof(struct ComponentPosition)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if (newPosComponent==NULL) {
        //write the error to the logfile
        WriteError("Could not allocate memory for position components! ");
        return NULL;
    }

    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        //set x & y position
        newPosComponent[i].x = -1000;
        newPosComponent[i].y = -1000;
        for (j = 0; j < NUM_OF_POSITION_HISTORY; ++j) {
            newPosComponent[i].oldx[j] = -1000;
            newPosComponent[i].oldy[j] = -1000;
        }
        newPosComponent[i].xOffset = 0;
        newPosComponent[i].yOffset = 0;
    }

    //return the pointer to the position components
    return newPosComponent;
}

void ComponentPosition_AllocateMoreMemory(Scene *scene,int componentIndex) {
    int j = 0;
    ComponentPosition *newComponentPosition = realloc((ComponentPosition*)scene->components[componentIndex].data,sizeof(struct ComponentPosition)*scene->maxEntities);
    if (newComponentPosition == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for new position components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentPosition[j].x = 0;
        newComponentPosition[j].y = 0;
        newComponentPosition[j].xOffset = 0;
        newComponentPosition[j].yOffset = 0;
    }
    //point the pointer to the new data
    scene->components[componentIndex].data = newComponentPosition;
}

void ComponentPosition_Free(ComponentPosition *positionComponent) {
    //if the position component is not NULL
    if (positionComponent!=NULL) {
        //free it
        free(positionComponent);
    }
}

void ComponentPosition_SetPosition(ComponentPosition *positionComponent,Uint32 entity,float x,float y) {
    if (positionComponent!=NULL) {
        positionComponent[entity].x = x;
        positionComponent[entity].y = y;
    }
    else{
        //write the error to the logfile
        WriteError("Cannot set position, parameter: 'ComponentPosition *positionComponent' is NULL!");
        return;
    }
}


void ComponentPosition_SetOffset(ComponentPosition *positionComponent,Uint32 entity,float x,float y) {
    if (positionComponent!=NULL) {
        positionComponent[entity].xOffset = x;
        positionComponent[entity].yOffset = y;
    }
    else{
        //write the error to the logfile
        WriteError("Cannot set position offset, parameter: 'ComponentPosition *positionComponent' is NULL!");
        return;
    }
}

void ComponentPosition_AddOldPositionToStack(ComponentPosition *positionComponent, Uint32 entity) {
    if (positionComponent!=NULL) {
        // switch the positions in the stack
        for (int i = 0; i < NUM_OF_POSITION_HISTORY - 1; ++i) {
            positionComponent[entity].oldx[i+1] = positionComponent[entity].oldx[i];
            positionComponent[entity].oldy[i+1] = positionComponent[entity].oldy[i];
        }
        positionComponent[entity].oldx[0] = positionComponent[entity].x;
        positionComponent[entity].oldy[0] = positionComponent[entity].y;
    }
    else{
        //write the error to the logfile
        WriteError("parameter: 'ComponentPosition *positionComponent' is NULL!");
        return;
    }
}
