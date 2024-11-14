#include "System.h"
#include "SystemInput.h"
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"


//define a mask for the move system. It requires a position and velocity component.
//it works on SET1 components, so we mark that as well in the define
#define SYSTEM_MOVE_MASK_SET1 (COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY)

#define COMPONENT_NO_INDEX -100

//local global pointers to the data
static ComponentPosition *posComponents = NULL;
static ComponentVelocity *velComponents = NULL;
static Scene *scn = NULL;
//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers() {
    if (scn == NULL) {
        return;
    }
    //ERROR handling is done in the scene.c file for the components. If a realloc fail, the system will shut down there.

    //get the position components pointer
    posComponents = (ComponentPosition*)Scene_GetComponent(scn,COMPONENT_SET1_POSITION);

    //get the velocity components pointer
    velComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!                                                      !!!
// !!!   Always call this function when changing a scene!   !!!
// !!!                                                      !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Function to initialize the move system
//
// This is used to ensure that the game will not crash when
// changing scenes. This is because scenes can have different indexes
// for the components.
int SystemMove_Init(void *scene) {
    systemFailedToInitialize=0;

    WriteDebug("Initializing Move System...");
    //if the passed entity manager is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Move system failed to initialize: *scene is NULL!");

        //mark that the system failed to initialize
        systemFailedToInitialize = 1;
        //exit the function
        return 0;
    }
    //typecast the void *scene to a Scene* pointer
    scn = (Scene*)scene;

    //check if the scene has position components
    posComponents = (ComponentPosition*)Scene_GetComponent(scn,COMPONENT_SET1_POSITION);
    //if not
    if (posComponents == NULL) {
        //log it as an error
        WriteError("Move system failed to initialize: Scene does not have position components (COMPONENT_SET1_POSITION)!");
        systemFailedToInitialize = 1;
        return 0;
    }

    // //check if the scene has velocity components
    velComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);
    //if not
    if (velComponents == NULL) {
        //log it as an error
        WriteError("Move system failed to initialize: Scene does not have velocity components (COMPONENT_SET1_VELOCITY)!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //flag that the initialization went ok
    systemFailedToInitialize = 0;

    //log that the move system was successfully initialized
    WriteDebug("Initializing Move System... DONE!");

    //return 1, successfully initialized the system
    return 1;
}

void SystemMove_Update() {
    if (scn == NULL) {
       return;
    }
    //if the component pointers have been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }
}

void SystemMove_UpdateEntity(Uint32 entity) {

    //if the move system failed to initialize
    if (systemFailedToInitialize == 1) {
        //return out of the function
        return;
    }

    //if the entity has the position and velocity component
    if (scn->entities[entity].componentSet1 & SYSTEM_MOVE_MASK_SET1) {
        ComponentPosition_AddOldPositionToStack(posComponents,entity);
        //update the entity position
        posComponents[entity].x += (velComponents[entity].x * DeltaTimer_GetDeltaTime());
        posComponents[entity].y += (velComponents[entity].y * DeltaTimer_GetDeltaTime());

        //if the x velocity is larger than 0
        if (velComponents[entity].x > 0) {
            //decrease speed with friction
            velComponents[entity].x-=velComponents[entity].friction;
            //if the velocity shot over to be negative
            if (velComponents[entity].x<0) {
                //set the velocity to 0
                velComponents[entity].x = 0;
            }
        }

        //if the x velocity is lower than 0
        if (velComponents[entity].x < 0) {
            //decrease speed with friction
            velComponents[entity].x+=velComponents[entity].friction;
            //if the velocity shot over to be positive
            if (velComponents[entity].x>0) {
                //set the velocity to 0
                velComponents[entity].x = 0;
            }
        }

        //if the y velocity is larger than 0
        if (velComponents[entity].y > 0) {
            //decrease speed with friction
            velComponents[entity].y-=velComponents[entity].friction;
            //if the velocity shot over to be negative
            if (velComponents[entity].y<0) {
                //set the velocity to 0
                velComponents[entity].y = 0;
            }
        }

        //if the y velocity is lower than 0
        if (velComponents[entity].y < 0) {
            //decrease speed with friction
            velComponents[entity].y+=velComponents[entity].friction;
            //if the velocity shot over to be positive
            if (velComponents[entity].y>0) {
                //set the velocity to 0
                velComponents[entity].y = 0;
            }
        }

        //Uncomment line to test
        //printf("\nEntity:%d posx:%.2f posy:%.2f ",entity,posComponents[entity].x,posComponents[entity].y);
    }
}

void SystemMove_MoveSystem() {
    //move system is not allocating anything, so we leave it empty
}
