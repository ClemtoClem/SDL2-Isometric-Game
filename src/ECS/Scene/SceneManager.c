#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "SceneManager.h"
#include "Scene.h"

SceneManager *SceneManager_CreateNewSceneManager() {
    SceneManager *sm = malloc(sizeof(struct SceneManager));
    if (sm == NULL) {
        //write the error to the log file
        WriteError("Could not allocate memory for a new scene manager!");
        return NULL;
    }
    //allocate scene pointers, note the: "struct Scene*"
    sm->scenes = malloc(sizeof(struct Scene*)*NUM_INITIAL_SCENES);
    if (sm->scenes == NULL) {
        WriteError("Could not allocate memory for scenes!");
        free(sm);
        return NULL;
    }
    sm->numScenes = 0;
    sm->maxScenes = NUM_INITIAL_SCENES;
    sm->activeScene = SCENE_MANAGER_NO_ACTIVE_SCENE;
    return sm;
}

void SceneManager_SetActiveScene(SceneManager *sceneManager,char *sceneName) {
    int i = 0;

    //if the passed scene manager is NULL
    if (sceneManager==NULL) {
        //write it to the log file
        WriteError("Parameter 'SceneManager *sceneManager' is NULL!");
        return;
    }
    //loop through all the scenes
    for (i = 0; i < sceneManager->numScenes; ++i) {
        //if the scene was found
        if (strcmp(sceneManager->scenes[i]->name,sceneName)==0) {
            //set the it to the active one
            sceneManager->activeScene = i;

            //Initialize the scene systems
            Scene_InitSystemsInScene(sceneManager->scenes[i]);

            //exit the function
            return;
        }
    }
    //if the scene was not found, log a warning about it.
    WriteError("Scene:%s was not found! Active scene was not changed!",sceneName);
}

void SceneManager_CreateSceneInSceneManager(SceneManager *sceneManager,char *sceneName) {
    Scene **newScenes = NULL;

    //if the passed scene manager is NULL
    if (sceneManager==NULL) {
        //write it to the log file
        WriteError("Parameter: 'SceneManager *sceneManager' is NULL!");
        return;
    }

    //if more memory is needed
    if (sceneManager->numScenes>=sceneManager->maxScenes) {
        //add five more scenes
        sceneManager->maxScenes+=5;
        //re-allocate memory for the scene pointers
        newScenes = realloc(sceneManager->scenes,sizeof(struct Scene*)*sceneManager->maxScenes);

        //if memory allocation failed
        if (newScenes==NULL) {
            WriteError("Could not allocate memory for more scenes! Scene:%s was not added!",sceneName);
            sceneManager->maxScenes-=5;
            return;
        }

        //point the scene pointer to the new memory
        sceneManager->scenes = newScenes;
    }

    //create a new scene
    //if something went wrong, the ecsCreateNewScene() will log the error to file
    sceneManager->scenes[sceneManager->numScenes] = Scene_CreateNewScene(sceneName);

    //if all went well
    if (sceneManager->scenes[sceneManager->numScenes] != NULL) {
        //increase number of scenes with one
        sceneManager->numScenes++;
    }
}

void SceneManager_RemoveSceneFromSceneManager(SceneManager *sceneManager,char *sceneName) {
    int i = 0;

    //if the passed in scene manager is NULL
    if (sceneManager==NULL) {
        //write it to the log file
        WriteError("Parameter: 'SceneManager *sceneManager' is NULL!");
        return;
    }

    //loop through all the scenes
    for (i = 0; i < sceneManager->numScenes; ++i) {
        //if the scene was found
        if (strcmp(sceneManager->scenes[i]->name,sceneName)==0) {
            //if there is only one scene or it's the last scene
            if (sceneManager->numScenes==1 || i==sceneManager->numScenes) {
                //Free the scene
                Scene_FreeScene(sceneManager->scenes[i]);

                //decrease number of scenes.
                sceneManager->numScenes--;

                //we removed the scene, exit the function
                return;
            }

            //free the current scene
            Scene_FreeScene(sceneManager->scenes[i]);

            //copy the last scene to the current scene position.
            sceneManager->scenes[i] = sceneManager->scenes[sceneManager->numScenes];
            //We don't remove any pointers in the last position, since they will be overwritten
            //next time a scene is added to that position in the list

            //decrease number of scenes.
            sceneManager->numScenes--;
            //we removed the scene, exit the function
            return;
        }
    }
    //if we reach the end without removing anything
    //Log that the scene was not removed, since it could not be found.
    WriteWarning("Scene:%s was not found. No scene was removed!",sceneName);
    }

Scene *SceneManager_GetPointerToScene(SceneManager *sceneManager,char *sceneName) {
    int i = 0;
    //if the passed in scene manager is NULL
    if (sceneManager==NULL) {
        //write it to the log file
        WriteError("Parameter: 'SceneManager *sceneManager' is NULL!");
        return NULL;
    }
    //if the sceneName is NULL
    if (sceneName == NULL) {
        //write it to the log file
        WriteError("Parameter: 'char *sceneName' is NULL!");
        return NULL;
    }
    //loop through all the scenes
    for (i = 0; i < sceneManager->numScenes; ++i) {
        //if the scene was found
        if (strcmp(sceneManager->scenes[i]->name,sceneName)==0) {
            //return it
            return sceneManager->scenes[i];
        }
    }
    //if the scene was not found, log a warning about it.
    WriteError("Scene:%s was not found! Returning NULL",sceneName);
    
    //return NULL
    return NULL;
}

void SceneManager_RunActiveScene(SceneManager *sceneManager) {
    //event to handle keyboard if the scene is without keyboard component and input system
    SDL_Event event;
    int handleExit = 0;

    //if the scene is NULL
    if (sceneManager == NULL) {
        //write error to the log file and exit out of the function
        WriteError("parameter: 'SceneManager *sceneManager' is NULL");
        return;
    }
    //if there is no active scene
    if (sceneManager->activeScene == SCENE_MANAGER_NO_ACTIVE_SCENE) {
        //log it as info
        WriteError("No active scene is set! (activeScene = SCENE_MANAGER_NO_ACTIVE_SCENE )");
    }

    //If the scene is NULL
    if (sceneManager->scenes[sceneManager->activeScene] == NULL) {
        //log it as an error
        WriteError("Active Scene:%d is NULL! Cannot switch scene.",sceneManager->activeScene);
    }

    //if the scene is without keyboard component
    if (sceneManager->scenes[sceneManager->activeScene]->sceneHasInputKeyboardComponent == 0) {
        WriteError("Scene:%s has no keyboard component! - Functionality to exit scene by pressing ESC will be added\n",sceneManager->scenes[sceneManager->activeScene]->name);
        handleExit = 1;
    }
    //if the scene is without an input system
    if (sceneManager->scenes[sceneManager->activeScene]->sceneHasInputSystem == 0) {
        WriteError("Scene:%s has no input system! - Functionality to exit scene by pressing ESC will be added\n",sceneManager->scenes[sceneManager->activeScene]->name);
        handleExit = 1;
    }

    //if systems for the scene has failed to initialize
    if (sceneManager->scenes[sceneManager->activeScene]->systemInitFailed == 1) {
        //log it as an error
        WriteError("Cannot switch scene! Systems has failed to initialize for scene:%s!",sceneManager->scenes[sceneManager->activeScene]->name);
    }
    //as long as exitScene is false
    while (!sceneManager->scenes[sceneManager->activeScene]->exitScene) {
        //If the scene is without keyboard input, add exit handling to the scene
        if (handleExit == 1) {
            //Poll events
            while (SDL_PollEvent(&event)) {
                //if the user is calling SDL_QUIT
                if (event.type == SDL_QUIT) {
                    sceneManager->scenes[sceneManager->activeScene]->exitScene = 1;
                }
                //if a key was released
                if (event.type == SDL_KEYUP) {
                    //if the user is pressing escape
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        sceneManager->scenes[sceneManager->activeScene]->exitScene = 1;
                    }
                }
            }
        }

        //if the scene should utilize less CPU
        if (sceneManager->scenes[sceneManager->activeScene]->consumeLessCPU==1) {
            //don't be a CPU hog! Delay a little bore processing next frame
            SDL_Delay(1);
        }

        //update the delta timer
        DeltaTimer_Update();

        //update all the systems in the scene
        Scene_UpdateSystemsInScene(sceneManager->scenes[sceneManager->activeScene]);
    }
}

void SceneManager_FreeSceneManager(SceneManager *sceneManager) {
    int i = 0;    //if the scene manager has allocated memory
    if (sceneManager != NULL) {
        //if the list of scenes is allocated
        if (sceneManager->scenes != NULL) {
            //loop through all the scenes
            for (i = 0; i < sceneManager->numScenes; ++i) {
                //if the scene is allocated
                if (sceneManager->scenes[i] != NULL) {
                    //free the scene
                    Scene_FreeScene(sceneManager->scenes[i]);
                }
            }
            //free the scenes
            free(sceneManager->scenes);
        }
        //free the scene manager
        free(sceneManager);
    }
}
