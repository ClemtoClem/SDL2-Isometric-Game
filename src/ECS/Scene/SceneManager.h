#ifndef __SCENE_MANAGER_H
#define __SCENE_MANAGER_H

#include "Scene.h"

#define NUM_INITIAL_SCENES 5

#define SCENE_MANAGER_NO_ACTIVE_SCENE -1

typedef struct SceneManager {
    int numScenes;          //current number of scenes
    int maxScenes;          //current max scenes
    int activeScene;        //current active scene
    Scene **scenes;        //list of scenes (pointer to an array with the different scenes)
} SceneManager;

SceneManager *SceneManager_CreateNewSceneManager();
void SceneManager_SetActiveScene(SceneManager *sceneManager,char *sceneName);
void SceneManager_CreateSceneInSceneManager(SceneManager *sceneManager,char *sceneName);
void SceneManager_RemoveSceneFromSceneManager(SceneManager *sceneManager,char *sceneName);
Scene *SceneManager_GetPointerToScene(SceneManager *sceneManager,char *sceneName);
void SceneManager_RunActiveScene(SceneManager *sceneManager);
void SceneManager_FreeSceneManager(SceneManager *sceneManager);

#endif // __SCENE_MANAGER_H
