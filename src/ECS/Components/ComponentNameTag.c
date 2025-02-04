#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../logger.h"
#include "../Scene/Scene.h"
#include "ComponentNameTag.h"

ComponentNameTag *ComponentNameTag_New() {
    int i = 0;    ComponentNameTag *nameTag = malloc(sizeof(struct ComponentNameTag)*NUM_INITIAL_ENTITIES);

    if (nameTag == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate memory for name tag components! ");
        return NULL;
    }

    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        //set the name tag to NULL
        nameTag->name = NULL;
    }
    //return the name tag
    return nameTag;
}
void ComponentNameTag_AllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentNameTag *newComponentNameTag = realloc((ComponentNameTag*)scene->components[componentIndex].data,sizeof(struct ComponentNameTag)*scene->maxEntities);
    if (newComponentNameTag == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for name tag components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentNameTag[j].name = NULL;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentNameTag;
}

void ComponentNameTag_SetName(ComponentNameTag *nameTagComponent,Uint32 entity,char *name) {
    int length = 0;
    //if the name tag exist
    if (nameTagComponent==NULL) {
        //write the error to the logfile
        WriteError("Parameter: 'ComponentNameTag *nameTagComponent' is NULL ");
        return;
    }
    //if the provided name is not NULL
    if (name==NULL) {
        //write the error to the logfile
        WriteError("Parameter: 'char *name' is NULL ");
        return;
    }
    //if the name is allocated, then the user is changing the name.
    if (nameTagComponent[entity].name != NULL) {
        //free it
        free(nameTagComponent[entity].name);
    }

    //get the length of the name
    length = strlen(name)+1;

    //allocate memory for the name
    nameTagComponent[entity].name = malloc(sizeof(char)*length);

    //if the memory allocation failed
    if (nameTagComponent[entity].name == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate memory for name:%s on entity:%d",name,entity);
        return;
    }
    //set the name
    sprintf(nameTagComponent[entity].name,"%s",name);
}

void componentNameTagGetEntityNameTag(ComponentNameTag *nameTagComponent,Uint32 entity,char *tagName) {
    if (nameTagComponent == NULL) {
        WriteError("Parameter: 'ComponentNameTag *nameTagComponent' is NULL!");
    }
    if (tagName == NULL) {
        WriteError("Parameter: 'char *tagName' is NULL!");
    }

    //if there is no name on the component
    if (nameTagComponent[entity].name==NULL) {
        //simply write an end line character to the string
        tagName[0] = '\0';
    }
    //otherwise
    else{
        //write the name tag to you tagName pointer
        sprintf(tagName,"%s",nameTagComponent[entity].name);
    }
}

void ComponentNameTag_Free(ComponentNameTag *nameTagComponent,Uint32 maxEntities) {
    Uint32 i = 0;    //if the position component is not NULL
    if (nameTagComponent!=NULL) {
        for (i = 0; i < maxEntities; ++i) {
            //if the name is not NULL
            if (nameTagComponent[i].name!=NULL) {
                //free the name
                free(nameTagComponent[i].name);
            }
        }
        //free the tag component
        free(nameTagComponent);
    }
}

int ComponentNameTag_GetEntityIDFromEntityByName(ComponentNameTag *nameTagComponents,char *entityName,Uint32 numEntities) {
    Uint32 i = 0;
    if (nameTagComponents == NULL) {
        WriteError("Parameter: 'ComponentNameTag *nameTagComponents' is NULL!");
        return -1;
    }
    if (entityName == NULL) {
        WriteError("Parameter: 'char *entityName' is NULL!");
        return -1;
    }

    for (i = 0; i < numEntities; ++i) {
        //make sure that the name is not a NULL pointer
        if (nameTagComponents[i].name!=NULL) {
            //if the name tag of the component matches the entity name
            if (strcmp(nameTagComponents[i].name,entityName)==0)
            {
                //return the index
                return i;
            }
        }
    }
    return -1;
}
