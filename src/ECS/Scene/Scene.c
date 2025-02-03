
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <SDL2/SDL.h>
#include "../../IsoEngine/isoEngine.h"
#include "../../DeltaTimer.h"
#include "../../logger.h"
#include "../Entity/Entity.h"
#include "Scene.h"

//function prototypes, allowing the functions to be used before they are defined.
static void freeComponentsFromScene(Scene *scene);
static void freeSystemsFromScene(Scene *scene);
static void allocateMoreMemoryForComponents(Scene *scene);
static void moveEntityIndexInComponentsAfterEntityRemove(Scene* scene, Uint32 entityID);

Scene *Scene_CreateNewScene(char *name) {
    Uint32 i = 0;    //create the entity manager
    Scene *scene = malloc(sizeof(struct Scene));

    //if memory allocation for the scene failed
    if (scene == NULL) {
        //log the error and return NULL.
        WriteError("Could not allocate memory for a new scene!");
        return NULL;
    }
    //if no name was passed to the function
    if (name == NULL) {
        //Set the scene name to "newmap"
        snprintf(scene->name,SCENE_NAME_LENGTH,"Newmap");
    }
    //otherwise
    else{
        //Set the scene name
        snprintf(scene->name,SCENE_NAME_LENGTH,"%s",name);
    }

    //allocate memory for the entities
    scene->entities = malloc(sizeof(Entity)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if (scene->entities == NULL) {
        //free the entity manager
        free(scene);

        //log the error
        WriteError("Could not allocate memory for: %d, entities!",NUM_INITIAL_ENTITIES);
        
        //return NULL
        return NULL;
    }
    //initialize the values
    scene->numEntities = 0;
    scene->maxEntities = NUM_INITIAL_ENTITIES;
    scene->memallocFailed = 0;
    scene->deleteScene = 0;
    scene->sceneHasInputKeyboardComponent = 0;
    scene->sceneHasInputMouseComponent = 0;
    scene->sceneHasInputSystem = 0;
    scene->exitScene = 0;
    scene->consumeLessCPU = 0;
    scene->isoEngine = NULL;
    scene->componentPointersReallocated = 0;

    //loop through all entities
    for (i = 0;i < scene->maxEntities; ++i) {
        //initialize its components to NONE.
        scene->entities[i].componentSet1 = COMPONENT_NONE;
        //scene->entities[i].componentSet2 = COMPONENT_NONE;
        scene->entities[i].id = 0;
    }

    //allocate memory for the components
    scene->components = malloc(sizeof(Component)*COMPONENT_TYPE_COUNT);

    //If memory allocation for the components failed
    if (scene->components == NULL) {

        //free the entity list
        free(scene->entities);

        //free the entity manager
        free(scene);

        //log the error
        WriteError("Could not allocate memory for components!");
        
        //return NULL
        return NULL;
    }
    //set number of components to 0
    scene->numComponents = 0;
    //set max components to the component type count
    scene->maxComponents = COMPONENT_TYPE_COUNT;

    //set component type to NONE for all allocated components
    for (i = 0; i < scene->maxComponents; ++i) {
        scene->components[i].type = COMPONENT_NONE;
        scene->components[i].data = NULL;
    }

    //allocate memory for systems
    scene->systems = malloc(sizeof(struct System)*NUM_INITIAL_SYSTEMS);

    //if the memory allocation for the systems failed
    if (scene->systems == NULL) {
        //free the components
        free(scene->components);

        //free the entity list
        free(scene->entities);

        //free the entity manager
        free(scene);

        //log the error
        WriteError("Could not allocate memory for systems!");
        
        //return NULL
        return NULL;
    }

    //initialize the system variables
    scene->numSystems = 0;
    scene->maxSystems = NUM_INITIAL_SYSTEMS;
    scene->systemInitFailed = 0;

    //initialize all systems to none
    for (i = 0; i < NUM_INITIAL_SYSTEMS; ++i) {
        scene->systems[i].type = SYSTEM_NONE;
        scene->systems[i].init = NULL;
        scene->systems[i].updateEntity = NULL;
        scene->systems[i].update = NULL;
    }

    //return the entity manager
    return scene;
}

int Scene_AddComponentToScene(Scene *scene, ComponentType componentType) {
    Uint32 i = 0;
    char componentName[200];

    //get the component name (stored in the componentName variable)
    ESC_GetComponentName(componentType, componentName);

    //if the passed pointer to the scene is NULL
    if (scene == NULL) {
        //log the error and return NULL.
        WriteError("Scene *scene is NULL!");
        return -1;
    }

    //if the components in the scene are not allocated
    if (scene->components == NULL) {
        //log the error and return NULL.
        WriteError("scene->components is NULL!");
        return -1;
    }

    //check if the component type already added to the scene
    for (i = 0; i < scene->numComponents; ++i) {
        //if so
        if (scene->components[i].type == componentType) {
            //log the error and return 0.
            WriteError("ComponentType: %s, already added to the scene!", componentName);
            return 0;
        }
    }

    //// POSITION COMPONENT
    if (componentType == COMPONENT_SET1_POSITION) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentPosition_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;
            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// VELOCITY COMPONENT
    else if (componentType == COMPONENT_SET1_VELOCITY) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentVelocity_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;

            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// KEYBOARD COMPONENT
    else if (componentType == COMPONENT_SET1_KEYBOARD) {
        scene->components[scene->numComponents].type = componentType;
        scene->components[scene->numComponents].data = componentNewInputKeyboard();
        if (scene->components[scene->numComponents].data == NULL) {
            scene->memallocFailed=1;
            return -1;
        }
        //increase number of components
        scene->numComponents++;
        scene->sceneHasInputKeyboardComponent = 1;
        return 1;
    }
    //// MOUSE COMPONENT
    else if (componentType == COMPONENT_SET1_MOUSE) {
        scene->components[scene->numComponents].type = componentType;
        scene->components[scene->numComponents].data = ComponentInputMouse_New();
        if (scene->components[scene->numComponents].data == NULL) {
            scene->memallocFailed=1;
            return -1;
        }
        //increase number of components
        scene->numComponents++;
        scene->sceneHasInputMouseComponent = 1;
        return 1;
    }
    //// RENDER2D COMPONENT
    else if (componentType == COMPONENT_SET1_RENDER2D) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentRender2D_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;

            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// NAME TAG COMPONENT
    else if (componentType == COMPONENT_SET1_NAMETAG) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentNameTag_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;

            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// NAME TAG COMPONENT
    else if (componentType == COMPONENT_SET1_COLLISION) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentCollision_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;

            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// NAME TAG COMPONENT
    else if (componentType == COMPONENT_SET1_ANIMATION) {
        //set the component type
        scene->components[scene->numComponents].type = componentType;

        //set the component data
        scene->components[scene->numComponents].data = ComponentAnimation_New();

        //if the data is NULL; return -1. An error was written to the log file
        if (scene->components[scene->numComponents].data == NULL) {
            //flag that memory allocation has failed
            scene->memallocFailed=1;

            return -1;
        }
        //increase number of components
        scene->numComponents++;
        return 1;
    }
    //// COMPONENT NOT IMPLEMENTED
    else {
        //log that the component is not implemented yet
        WriteError("ComponentType:%s with bit position: %d, has not been implemented, component NOT added!",componentName,ESC_GetComponentBit(componentType));
    }
    //return
    return -2;
}
static void freeSystemsFromScene(Scene *scene) {
    Uint32 i = 0;    if (scene==NULL) {
        WriteError("Scene* scene is NULL!");
        return;
    }
    if (scene->systems == NULL) {
        return;
    }
    //loop through the systems
    for (i = 0; i < scene->numSystems; ++i) {
        //free the system
        scene->systems[i].free();
    }
}
static void freeComponentsFromScene(Scene *scene) {
    Uint32 i = 0;
    char componentName[200];
    if (scene==NULL) {
        WriteError("Scene* scene is NULL!");
        return;
    }
    if (scene->components == NULL) {
        WriteError("scene->components NULL!");
        return;
    }
    //Loop through all components & free them
    for (i = 0; i < scene->numComponents; ++i) {
        //// POSITION COMPONENT
        if (scene->components[i].type == COMPONENT_SET1_POSITION) {
            ComponentPosition_Free((ComponentPosition*)scene->components[i].data);
        }
        //// VELOCITY COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_VELOCITY) {
            ComponentVelocity_Free((ComponentVelocity*)scene->components[i].data);
        }
        //// INPUT KEYBOARD COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_KEYBOARD) {
            componentFreeInputKeyboard((ComponentInputKeyboard*)scene->components[i].data,scene->maxEntities);
        }
        //// INPUT MOUSE COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_MOUSE) {
            ComponentInputMouse_Free((ComponentInputMouse*)scene->components[i].data,scene->maxEntities);
        }
        //// RENDER 2D COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_RENDER2D) {
            ComponentRender2D_Free((ComponentRender2D*)scene->components[i].data);
        }
        //// NAME TAG COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_NAMETAG) {
            ComponentNameTag_Free((ComponentNameTag*)scene->components[i].data,scene->maxEntities);
        }
        //// COLLISION COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_COLLISION) {
            ComponentCollision_Collision((ComponentCollision*)scene->components[i].data);
        }
        //// ANIMATION COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_ANIMATION) {
            ComponentAnimation_Free((ComponentAnimation*)scene->components[i].data,scene->maxEntities);
        }
        //// UNHANDLED COMPONENT
        else{
            //get the component name
            ESC_GetComponentName(scene->components[i].type,componentName);
            //log the error
            WriteError("ComponentType:%s with bit:%d is missing. Add free for it!",componentName,ESC_GetComponentBit(scene->components[i].type));
        }
    }
}

void Scene_FreeScene(Scene *scene) {
    //if the scene has allocated memory
    if (scene != NULL) {
        //free the entities
        if (scene->entities!=NULL) {
            free(scene->entities);
        }
        //free the components
        if (scene->components!=NULL) {
            //free each individual component inside the components pointer
            freeComponentsFromScene(scene);
            //free the components pointer
            free(scene->components);
        }
        //free the systems
        if (scene->systems!=NULL) {
            //free memory allocated by the systems
            freeSystemsFromScene(scene);
            //free the systems pointer
            free(scene->systems);
        }
        //free the isometric engine
        if (scene->isoEngine!=NULL) {
            //the IsoEngine_Free function also frees the scene->isoEngine pointer
            IsoEngine_Free(scene->isoEngine);
        }

        //free the scene
        free(scene);
    }
}

Uint32 Scene_AddEntityToScene(Scene* scene, Uint32 componentSet1) /*,Uint32 componentSet2)*/ {
    Entity *newEntityList = NULL;
    if (scene == NULL) {
        WriteError("Scene* scene is NULL!");
        return -1;
    }
    //if we're on the last entity, and no reallocation errors has occurred
    if (scene->numEntities >= scene->maxEntities && scene->memallocFailed == 0) {
        //try add another 1000 entities to the list
        scene->maxEntities+=1000;
        newEntityList = realloc(scene->entities,sizeof(struct Entity)*scene->maxEntities);

        //if the new entity list could not be created
        if (newEntityList==NULL) {
            //roll back max number of entities
            scene->maxEntities-=1000;
            WriteError("Failed to re-allocate memory for new entities!");
        }
        //if the entity list could be reallocated
        else{

            //allocate more memory for the components as well
            allocateMoreMemoryForComponents(scene);

            //if memory re-allocation failed
            if (scene->memallocFailed==1)
            {
                //free the new entity list. It us useless without components mapped to it.
                free(newEntityList);

                //TRY TO FREE SOME MEMORY HERE IF POSSIBLE, THEN HAVE A RE-TRY.

                //set the realloc Failed flag, stopping the engine from allocating more memory.
                scene->memallocFailed = 1;

                //roll-back number of entities.
                scene->maxEntities-=1000;

                //return -1
                return -1;
            }

            //point the entity list to the new one.
            scene->entities = newEntityList;

            //clear the components for the new entity
            scene->entities[scene->numEntities].componentSet1 = 0;
            //scene->entities[scene->numEntities].componentSet2 = 0;    //<- add when the number of components is more than 32

            //add the components to the entity
            scene->entities[scene->numEntities].componentSet1 = componentSet1;
            //scene->entities[scene->numEntities].componentSet2 = componentSet2;    //<- add when the number of components is more than 32

            //set the new entity
            scene->entities[scene->numEntities].id = scene->numEntities;
            scene->numEntities++;
            //return the entity index
            return scene->numEntities-1;
        }
    }
    else{
        //set the new entity
        scene->entities[scene->numEntities].componentSet1 = componentSet1;
        //scene->entities[scene->numEntities].componentSet2 = componentSet2;
        scene->entities[scene->numEntities].id = scene->numEntities;
        //increase number of entities
        scene->numEntities++;
        //return the entity index
        return scene->numEntities-1;
    }
    //if something went wrong
    return -1;
}

//each time the entities increase, so must the components.
static void allocateMoreMemoryForComponents(Scene *scene) {
    Uint32 i = 0;
    char componentName[200];
    /*ComponentPosition *newComponentPosition = NULL;
    ComponentVelocity *newComponentVelocity = NULL;
    ComponentInputKeyboard *newComponentInputKeyboard = NULL;
    ComponentInputMouse *newComponentInputMouse = NULL;
    ComponentRender2D *newComponentRender2D = NULL;
    ComponentNameTag *newComponentNameTag = NULL;
    ComponentCollision *newComponentCollision = NULL;*/

    //flag that the component pointers has been reallocated
    scene->componentPointersReallocated = 1;

    for (i = 0; i < scene->numComponents; ++i) {
        //// POSITION COMPONENT
        if (scene->components[i].type == COMPONENT_SET1_POSITION) {
            ComponentPosition_AllocateMoreMemory(scene,i);
        }
        //// VELOCITY COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_VELOCITY) {
            ComponentVelocity_AllocateMoreMemory(scene,i);
        }
        //// KEYBOARD COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_KEYBOARD) {
            componentInputKeyboardAllocateMoreMemory(scene,i);
        }
        //// MOUSE COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_MOUSE) {
            ComponentInputMouse_AllocateMoreMemory(scene,i);
        }
        //// RENDER 2D COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_RENDER2D) {
            ComponentRender2D_AllocateMoreMemory(scene,i);
        }
        //// NAMETAG COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_NAMETAG) {
            ComponentNameTag_AllocateMoreMemory(scene,i);
        }
        //// COLLISION COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_COLLISION) {
            ComponentCollision_AllocateMoreMemory(scene,i);
        }
        //// ANIMATION COMPONENT
        else if (scene->components[i].type == COMPONENT_SET1_ANIMATION) {
            ComponentAnimation_AllocateMoreMemory(scene,i);
        }
        //If the component has not yet been added to the function
        else{
            //log the error to file
            ESC_GetComponentName(scene->components[i].type,componentName);
            WriteError("ComponentType:%s with bit:%d has not been added here for more memory allocation!!",componentName,ESC_GetComponentBit(scene->components[i].type));
            //flag that memory allocation has failed
            scene->memallocFailed = 1;
            return;
        }
    }
}

void Scene_RemoveEntityFromScene(Scene* scene, Uint32 entityID) {
    Uint32 tmpID;
    //if the passed entity manager is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Scene *scene is NULL!");
        //exit the function
        return;
    }

    //Failsafe, if there are no entities, simply return
    if (scene->numEntities <= 0) {
        return;
    }

    //if there is only one entity, or if the entity to remove is the last one
    if (scene->numEntities==1 || entityID == scene->numEntities) {
        //printf("entity is the last one, or the only one... No entity index component moving is needed!");
        //Decrease number of entities with 1
        //The entity will be overwritten the next time a new entity is created.
        scene->numEntities--;
    }
    //if there are multiple entities
    else{
        //copy the entity ID from the entity to be removed
        tmpID = scene->entities[entityID].id;

        //move the entity last in the array to the slot of the entity that was removed.
        scene->entities[entityID] = scene->entities[scene->numEntities-1];

        //change its entity ID to its new slot position.
        scene->entities[entityID].id = tmpID;

        //Do the same in all components as well.
        moveEntityIndexInComponentsAfterEntityRemove(scene,entityID);

        //decrease number of entities.
        scene->numEntities--;
    }
}

static void moveEntityIndexInComponentsAfterEntityRemove(Scene* scene, Uint32 entityID) {
    //this is called inside Scene_RemoveEntityFromScene()
    //where a healthy check of if (scene == NULL ) just was performed, so we skip it here.
    Uint32 i = 0;
    char componentName[200];
    //pointers to the component types
    ComponentPosition *componentPosition = NULL;
    ComponentVelocity *componentVelocity = NULL;
    ComponentInputKeyboard *componentInputKeyboard = NULL;
    ComponentInputMouse *componentInputMouse = NULL;
    ComponentRender2D *componentRender2D = NULL;
    ComponentNameTag *componentNameTag = NULL;
    ComponentCollision *componentCollision = NULL;
    ComponentAnimation *componentAnimation = NULL;

    //loop through the components and move the component last in the array to the entity position that was removed
    for (i = 0; i < scene->numComponents; ++i) {
        if (scene->components[i].type == COMPONENT_SET1_POSITION) {
            componentPosition = (ComponentPosition*)scene->components[i].data;
            componentPosition[entityID] = componentPosition[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_VELOCITY) {
            componentVelocity = (ComponentVelocity*)scene->components[i].data;
            componentVelocity[entityID] = componentVelocity[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_KEYBOARD) {
            componentInputKeyboard =(ComponentInputKeyboard*)scene->components[i].data;
            componentInputKeyboard[entityID] = componentInputKeyboard[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_MOUSE) {
            componentInputMouse =(ComponentInputMouse*)scene->components[i].data;
            componentInputMouse[entityID] = componentInputMouse[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_RENDER2D) {
            componentRender2D = (ComponentRender2D*)scene->components[i].data;
            componentRender2D[entityID] = componentRender2D[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_NAMETAG) {
            componentNameTag = (ComponentNameTag*)scene->components[i].data;
            componentNameTag[entityID] = componentNameTag[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_COLLISION) {
            componentCollision = (ComponentCollision*)scene->components[i].data;
            componentCollision[entityID] = componentCollision[scene->numEntities-1];
        }
        else if (scene->components[i].type == COMPONENT_SET1_ANIMATION) {
            componentAnimation = (ComponentAnimation*)scene->components[i].data;
            componentAnimation[entityID] = componentAnimation[scene->numEntities-1];
        }
        //if the component is not yet handled by the function
        else{
            ESC_GetComponentName(scene->components[i].type,componentName);
            //log the error
            WriteError("ComponentType:%s with bit:%d is missing. Add it here!",componentName,ESC_GetComponentBit(scene->components[i].type));
        }
    }
}

Uint32 Scene_GetNumEntities(Scene *scene) {
    if (scene == NULL) {
        return -1;
    }
    return scene->numEntities;
}

Uint32 Scene_GetComponentIndex(Scene *scene,Uint32 componentFlag) {
    Uint32 i = 0;    //loop through all the components in the scene
    for (i = 0; i < scene->numComponents; ++i) {
        //if the component is found
        if (scene->components[i].type & componentFlag) {
            //return its index
            return i;
        }
    }
    //if the component is not found, return -1
    return -1;
}

void *Scene_GetComponent(Scene *scene, Uint32 componentFlag) {
    Uint32 i = 0;    //loop through all the components in the scene
    for (i = 0; i < scene->numComponents; ++i) {
        //if the component is found
        if (scene->components[i].type & componentFlag) {
            //return its index
            return scene->components[i].data;
        }
    }
    //if the component is not found, return NULL
    return NULL;
}

int Scene_AddSystemToScene(Scene *scene,SystemType systemType) {
    System *newSystems = NULL;
    char systemName[200];

    if (scene == NULL) {
        //log it as an error
        WriteError("Scene *scene is NULL!");
        //exit the function
        return 0;
    }
    //if we are on the last system
    if (scene->numSystems>=scene->maxSystems) {
        //increase max systems
        scene->maxSystems+=5;

        //re-allocate memory for systems
        newSystems = realloc(scene->systems,sizeof(System)*scene->maxSystems);

        //if memory allocation failed
        if (newSystems == NULL) {
            WriteError("Could not re-allocate memory for new systems!");

            //undo number of systems
            scene->maxSystems-=5;

            //return 0
            return 0;
        }

        //point the systems pointer to the new allocated memory
        scene->systems = newSystems;
    }
    //// MOVE SYSTEM
    if (systemType == SYSTEM_MOVE) {
        scene->systems[scene->numSystems].type = SYSTEM_MOVE;
        scene->systems[scene->numSystems].init = SystemMove_Init;
        scene->systems[scene->numSystems].updateEntity = SystemMove_UpdateEntity;
        scene->systems[scene->numSystems].update = SystemMove_Update;
        scene->systems[scene->numSystems].free = SystemMove_MoveSystem;
        scene->numSystems++;
    }
    //// INPUT SYSTEM
    else if (systemType == SYSTEM_INPUT) {
        scene->systems[scene->numSystems].type = SYSTEM_INPUT;
        scene->systems[scene->numSystems].init = SystemInput_Init;
        scene->systems[scene->numSystems].updateEntity = SystemInput_UpdateEntity;
        scene->systems[scene->numSystems].update = SystemInput_Update;
        scene->systems[scene->numSystems].free = SystemInput_Free;
        scene->numSystems++;
        scene->sceneHasInputSystem=1;
    }
    //// RENDER SYSTEM
    else if (systemType == SYSTEM_RENDER_ISOMETRIC_WORLD) {
        scene->systems[scene->numSystems].type = SYSTEM_RENDER_ISOMETRIC_WORLD;
        scene->systems[scene->numSystems].init = SystemRenderIsoMetricWorld_Init;
        scene->systems[scene->numSystems].update = SystemRenderIsoMetricWorld_Compute;
        scene->systems[scene->numSystems].updateEntity = SystemRenderIsoMetricWorld_SortEntity;
        scene->systems[scene->numSystems].free = SystemRenderIsoMetricWorld_Free;
        scene->numSystems++;
    }
    //// CONTROL ISOMETRIC WORLD SYSTEM
    else if (systemType == SYSTEM_CONTROL_ISOMETRIC_WORLD) {
        scene->systems[scene->numSystems].type = SYSTEM_CONTROL_ISOMETRIC_WORLD;
        scene->systems[scene->numSystems].init = SystemControlIsoWorld_Init;
        scene->systems[scene->numSystems].update = SystemControlIsoWorld_Compute;
        scene->systems[scene->numSystems].updateEntity = NULL;
        scene->systems[scene->numSystems].free = SystemControlIsoWorld_Free;
        scene->numSystems++;
    }
    //// CONTROL ENTITY SYSTEM
    else if (systemType == SYSTEM_CONTROL_ENTITY) {
        scene->systems[scene->numSystems].type = SYSTEM_CONTROL_ENTITY;
        scene->systems[scene->numSystems].init = SystemControlEntity_Init;
        scene->systems[scene->numSystems].update = SystemControlEntity_Compute;
        scene->systems[scene->numSystems].updateEntity = NULL;
        scene->systems[scene->numSystems].free = SystemControlEntity_Free;
        scene->numSystems++;
    }
    //// CONTROL ENTITY SYSTEM
    else if (systemType == SYSTEM_COLLISION) {
        scene->systems[scene->numSystems].type = SYSTEM_COLLISION;
        scene->systems[scene->numSystems].init = SystemCollision_Init;
        scene->systems[scene->numSystems].update = SystemCollision_Update;
        scene->systems[scene->numSystems].updateEntity = SystemCollision_UpdateEntity;
        scene->systems[scene->numSystems].free = SystemCollision_Free;
        scene->numSystems++;
    }
    else if (systemType == SYSTEM_ANIMATION) {
        scene->systems[scene->numSystems].type = SYSTEM_ANIMATION;
        scene->systems[scene->numSystems].init = SystemAnimation_Init;
        scene->systems[scene->numSystems].update = SystemAnimation_Update;
        scene->systems[scene->numSystems].updateEntity = SystemAnimation_UpdateEntity;
        scene->systems[scene->numSystems].free = SystemAnimation_Free;
        scene->numSystems++;
    }
    //// UNKNOWN SYSTEM
    else{
        //get the system name
        ESC_GetSystemName(systemType,systemName);
        //log the warning to file
        WriteError("System:%s is not handled. Add it here!",systemName);
    }
    return 1;
}

int Scene_InitSystemsInScene(Scene *scene) {
    Uint32 i = 0;
    //if the scene is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Scene *scene is NULL!");
        //exit the function
        return 0;
    }
    //loop through all the systems and initialize them
    for (i = 0; i < scene->numSystems; ++i) {
        //Initialize the system
        if (scene->systems[i].init(scene) == 0) {
            scene->systemInitFailed = 1;
            //if the initialization failed, return 0
            return 0;
        }
    }
    //All systems were initialized successfully, return 1
    return 1;
}

void Scene_UpdateSystemsInScene(Scene *scene) {
    Uint32 i = 0;
    Uint32 j = 0;
    //No scene == NULL check here, this will be called each game loop
    //so we want it as fast as possible

    //run the systems that don't require working on an entity
    for (i = 0; i < scene->numSystems; ++i) {
        //if the system has an update function
        if (scene->systems[i].update!=NULL) {
            //update the system
            scene->systems[i].update(scene);
        }
    }

    //loop through all entities in the scene
    for (i = 0; i < scene->numEntities; ++i) {
        //loop through all the systems in the scene
        for (j = 0; j < scene->numSystems; ++j) {
            //if the system for updating entities exist
            if (scene->systems[j].updateEntity != NULL) {
                //update the system, performing changes on the entities that match the required components
                scene->systems[j].updateEntity(i);
            }
        }
    }

    //if the component pointers were reallocated
    //the systems will now have updated the pointers
    if (scene->componentPointersReallocated == 1) {
        //Reset the pointers reallocated flag to 0
        scene->componentPointersReallocated = 0;
    }
}

void ESC_GetSystemName(SystemType systemType,char *name) {
    if (systemType == SYSTEM_MOVE) {
        sprintf(name,"SYSTEM_MOVE");
    }
    else if (systemType == SYSTEM_INPUT) {
        sprintf(name,"SYSTEM_INPUT");
    }
    else if (systemType == SYSTEM_RENDER_ISOMETRIC_WORLD) {
        sprintf(name,"SYSTEM_RENDER_ISOMETRIC_WORLD");
    }
    else if (systemType == SYSTEM_CONTROL_ISOMETRIC_WORLD) {
        sprintf(name,"SYSTEM_CONTROL_ISOMETRIC_WORLD");
    }
    else if (systemType == SYSTEM_CONTROL_ENTITY) {
        sprintf(name,"SYSTEM_CONTROL_ENTITY");
    }
    else if (systemType == SYSTEM_ANIMATION) {
        sprintf(name,"SYSTEM_ANIMATION");
    }
    else{
        sprintf(name," ESC_GetSystemName(id:%d) - System Not Named!",systemType);
    }
}

void ESC_GetComponentName(ComponentType componentType,char *name) {
    if (componentType == COMPONENT_SET1_POSITION) {
        sprintf(name,"COMPONENT_SET1_POSITION");
    }
    else if (componentType == COMPONENT_SET1_VELOCITY) {
        sprintf(name,"COMPONENT_SET1_VELOCITY");
    }
    else if (componentType == COMPONENT_SET1_MOUSE) {
        sprintf(name,"COMPONENT_SET1_MOUSE");
    }
    else if (componentType == COMPONENT_SET1_KEYBOARD) {
        sprintf(name,"COMPONENT_SET1_KEYBOARD");
    }
    else if (componentType == COMPONENT_SET1_MOUSE) {
        sprintf(name,"COMPONENT_SET1_MOUSE");
    }
    else if (componentType == COMPONENT_SET1_RENDER2D) {
        sprintf(name,"COMPONENT_SET1_RENDER2D");
    }
    else if (componentType == COMPONENT_SET1_NAMETAG) {
        sprintf(name,"COMPONENT_SET1_NAMETAG");
    }
    else if (componentType == COMPONENT_SET1_COLLISION) {
        sprintf(name,"COMPONENT_SET1_COLLISION");
    }
    else if (componentType == COMPONENT_SET1_ANIMATION) {
        sprintf(name,"COMPONENT_SET1_ANIMATION");
    }
    else{
        sprintf(name," getComponentName(bit:%d) - Component Not Named!",ESC_GetComponentBit(componentType));
    }
}

int ESC_GetComponentBit(ComponentType componentType) {
    char componentName[200];
    int position = 1;
    Uint32 i=1;     // start on the first bit: 0000 0000 0000 0000 0000 0000 0000 000X

    //every next bit in the binary system will have the double value to the previous bit:
    //0000 0000 0000 0000 0000 0000 0000 0001 = 1   (position = 1)
    //0000 0000 0000 0000 0000 0000 0000 0010 = 2   (position = 2)
    //0000 0000 0000 0000 0000 0000 0000 0100 = 4   (position = 3)
    //0000 0000 0000 0000 0000 0000 0000 1000 = 8   (position = 4)
    //etc..

    //therefore we simply increase i with i*=2;

    while (1) {
        //if the component is found
        if (i == (Uint32)componentType) {
            //return the current position
            return position;
        }

        //go to the next bit
        i*=2;

        //add the counter with 1
        position++;
        //if we've reached the last bit in the 32-bit integer)
        if (position>=32) {

            //the component was not found, and is probably not correctly initialized
            //so give a warning about that
            ESC_GetComponentName(componentType, componentName);
            WriteError("componentType:%s is not correctly initialized",componentName);
            
            //break out from the while loop
            break;
        }
    }
    return -1;
}

void Scene_SetCPUDelay(Scene *scene, int value) {
    if (scene == NULL) {
        //write error to the log file and exit out of the function
        WriteError("parameter 'Scene *scene' is NULL");
        return;
    }
    if (value == 1) {
        scene->consumeLessCPU = 1;
    }
    else if (value == 0) {
        scene->consumeLessCPU = 0;
    }
}

