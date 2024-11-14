#include <stdio.h>
#include "System.h"
#include "SystemCollision.h"
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"
#include "../../IsoEngine/isoEngine.h"
#include "SystemAnimation.h"

#define SYSTEM_ANIMATION_MASK_SET1 (COMPONENT_SET1_ANIMATION)

//local global pointer to the scene
static Scene *scn = NULL;

//local global pointer to the data
static ComponentAnimation *animComponents = NULL;
static ComponentRender2D *renderComponents = NULL;

//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers() {
    //if there is no scene
    if (scn == NULL) {
        //return out of the function
        return;
    }
    //Error handling is done in the scene.c file.

    //get the render 2D component pointer
    animComponents = (ComponentAnimation*)Scene_GetComponent(scn,COMPONENT_SET1_ANIMATION);

    //get the render 2D component pointer
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
}

int SystemAnimation_Init(void *scene) {
    systemFailedToInitialize = 0;

    WriteDebug("Initializing collision system...");

    if (scene == NULL) {
        WriteError("Animation system failed to initialize: Parameter 'void *scene' is NULL ");
        systemFailedToInitialize = 1;
        return 0;
    }

    scn = (Scene*)scene;

    //check if the scene has collision components
    animComponents = (ComponentAnimation*)Scene_GetComponent(scn,COMPONENT_SET1_ANIMATION);
    //if not
    if (animComponents == NULL) {
        //log the error
        WriteError("Animation system failed to initialize: The scene does not have animation components (COMPONENT_SET1_ANIMATION)");
        systemFailedToInitialize = 1;
        return 0;
    }
/*
    //check if the scene has render 2D components
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
    //if not
    if (renderComponents == NULL) {
        //log the error
        WriteError("Collision system failed to initialize: The scene does not have render 2D components (COMPONENT_SET1_RENDER2D)");
        systemFailedToInitialize = 1;
        return 0;
    }
    WriteDebug("Initializing Collision System... DONE"); */
    //return 1, successfully initialized the system
    return 1;
}

void SystemAnimation_Update() {
    //if the system failed to initialize
    if (systemFailedToInitialize == 1) {
        return;
    }
    //if the component pointers has been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        updateComponentPointers();
    }
}

void SystemAnimation_UpdateEntity(Uint32 entity) {
    int currentFrameIndex = 0;

    //if the system failed to initialize
    if (systemFailedToInitialize == 1) {
        return;
    }
    //if the entity has the animation component
    if (scn->entities[entity].componentSet1 & SYSTEM_ANIMATION_MASK_SET1) {
        //if the animation has any animations
        if (animComponents[entity].numAnimations >0) {
            //if it is time to go to the next frame
            if (Timer_Update(&animComponents[entity].animations[animComponents[entity].animationState].frameTime))
            {
                //go to the next frame
                animComponents[entity].animations[animComponents[entity].animationState].currentFrame++;

                //if the animation has reached its end
                if (animComponents[entity].animations[animComponents[entity].animationState].currentFrame >=
                   animComponents[entity].animations[animComponents[entity].animationState].numFrames)
                {
                    //reset the frame to the first one
                    animComponents[entity].animations[animComponents[entity].animationState].currentFrame = 0;
                }
                //copy the frame index, (for easier readability of the code on the Timer_Init row below)
                currentFrameIndex = animComponents[entity].animations[animComponents[entity].animationState].currentFrame;

                //set the timer duration to the frame's frame time
                Timer_Init(&animComponents[entity].animations[animComponents[entity].animationState].frameTime,
                animComponents[entity].animations[animComponents[entity].animationState].frames[currentFrameIndex].frameTimeMilliSeconds);
            }
        }
    }
}

void SystemAnimation_Free() {
    //the system is not allocation anything, so there is nothing to free
}
