#ifndef __SCENE_H
#define __SCENE_H

#include "../Entity/Entity.h"
#include "../Components/Component.h"
#include "../System/System.h"
#include "../../IsoEngine/isoEngine.h"

#define NUM_INITIAL_SYSTEMS  1
#define NUM_INITIAL_ENTITIES 1

#define SCENE_NAME_LENGTH 128

//scene struct
typedef struct Scene {
    char name[SCENE_NAME_LENGTH];   //Name of the scene
    Entity *entities;               //the entities in the scene
    Uint32 numEntities;             //current number of entities
    Uint32 maxEntities;             //current max allocated entities

    Component *components;          //the components available in the scene
    Uint32 numComponents;           //current number of components
    Uint32 maxComponents;           //current max allocated components

    System *systems;                //the registered systems in the scene
    Uint32 numSystems;              //current number of systems running
    Uint32 maxSystems;              //current max allocated systems

    int memallocFailed;             //if a memory allocation failure has occurred.
    int systemInitFailed;           //if a system has failed to initialize
    int deleteScene;                //if the scene should be deleted and removed from memory
    int exitScene;                  //if the user wants to exit the scene
    int consumeLessCPU;             //if the system should use less CPU ( SDL_Delay(1) )

    int sceneHasInputMouseComponent;        //if the scene has the mouse input component
    //If the scene does not have keyboard input, we have to add functionality to the scene so that it can be closed (Esc key or SLD_QUIT event)
    int sceneHasInputSystem;                //if the scene has an input system
    int sceneHasInputKeyboardComponent;     //if the scene has the keyboard input component
    int componentPointersReallocated;         //if the components in the scene was reallocated

    IsoEngine *isoEngine;                   //Pointer to isometric engine
} Scene;

[[nodiscard]] Scene *Scene_CreateNewScene(char *name);
void Scene_FreeScene(Scene *scene);
Uint32 Scene_AddEntityToScene(Scene* scene, Uint32 componentSet1);/*,Uint32 componentSet2);*/
int Scene_AddComponentToScene(Scene *scene, ComponentType componentType);
void Scene_RemoveEntityFromScene(Scene* scene, Uint32 entityID);
[[nodiscard]] Uint32 Scene_GetComponentIndex(Scene *scene,Uint32 componentFlag);
[[nodiscard]] void *Scene_GetComponent(Scene *scene,Uint32 componentFlag);

[[nodiscard]] Uint32 Scene_GetNumEntities(Scene *scene);
int Scene_AddSystemToScene(Scene *scene, SystemType systemType);
int Scene_InitSystemsInScene(Scene *scene);

void Scene_UpdateSystemsInScene(Scene *scene);
void ESC_GetSystemName(SystemType systemType,char *name);
void ESC_GetComponentName(ComponentType componentType,char *name);

[[nodiscard]] int ESC_GetComponentBit(ComponentType componentType);
void Scene_SetCPUDelay(Scene *scene, int value);

#endif // __scene_H


