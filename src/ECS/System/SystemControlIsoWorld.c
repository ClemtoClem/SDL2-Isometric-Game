#include <stdio.h>
#include <string.h>
#include "System.h"
#include "SystemControlIsoWorld.h"
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"
#include "../../IsoEngine/isoEngine.h"

//define a mask for the control isometric system. It requires the keyboard component
//it works on SET1 components, so we mark that as well in the define

//#define SYSTEM_CONTROL_ISO_MASK_SET1 (COMPONENT_SET1_KEYBOARD | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_MOUSE)
#define SYSTEM_CONTROL_ISO_MASK_SET1 (COMPONENT_SET1_KEYBOARD | COMPONENT_SET1_NAMETAG)

//local global variable for system failure
static int systemFailedToInitialize = 1;

//local global pointer to the components data
static ComponentInputKeyboard *keyboardInputComponents = NULL;
static ComponentInputMouse *mouseInputComponents = NULL;
static ComponentNameTag *nameTagComponents = NULL;

//local global pointer to the isometric engine
static IsoEngine *isoEngine = NULL;

//local global index of the entity having the keyboard states controlling the isometric rendering system
static int isometricControlEntityIndex = -1;

//local global pointer to the scene
static Scene *scn = NULL;

//list of the keys
static int keyScrollMapUp = -1;
static int keyScrollMapDown = -1;
static int keyScrollMapLeft = -1;
static int keyScrollMapRight = -1;
static int keyToggleGameMode = -1;

static int mouseWheelZoom = -1;
static int mouseLeftClick = -1;

static void updateComponentPointers() {
    if (scn == NULL) {
        return;
    }
    //ERROR handling is done in the scene.c file for the components. If a realloc fail, the system will shut down there.

    //get the pointer to the keyboard input components
    keyboardInputComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);

    //get the pointer to the mouse input components
    mouseInputComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);

    //get the pointer to the name tag components
    nameTagComponents = (ComponentNameTag*)Scene_GetComponent(scn,COMPONENT_SET1_NAMETAG);
}

int SystemControlIsoWorld_Init(void *scene) {
    systemFailedToInitialize=0;
    isometricControlEntityIndex = -1;

    WriteDebug("Initializing Isometric World Control System...");
    //if the passed entity manager is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Isometric World Control failed to initialize: *scene is NULL!");

        //mark that the system failed to initialize
        systemFailedToInitialize = 1;
        //exit the function
        return 0;
    }

    //typecast the void *scene to a Scene* pointer
    scn = (Scene*)scene;

    //if the scene has the isometric engine attached to it
    if (scn->isoEngine != NULL) {
        //set the local global isoEngine pointer to point to the isometric engine.
        isoEngine = scn->isoEngine;
    }
    //if the scene does not have the isometric engine
    else{
        //log it as an error
        WriteError("Isometric World Control system failed to initialize: The scene does not have the isoEngine!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the keyboard input components
    keyboardInputComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);
    //if the scene does not have a kayboard
    if (keyboardInputComponents == NULL) {
        //log it as an error
        WriteError("Isometric World Control system failed to initialize: Scene does not have 'Keyboard input' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the mouse input components
    mouseInputComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);
    if (mouseInputComponents == NULL) {
        //log it as an error
        WriteError("Isometric World Control system failed to initialize: Scene does not have 'Mouse input' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the name tag components
    nameTagComponents = (ComponentNameTag*)Scene_GetComponent(scn,COMPONENT_SET1_NAMETAG);
    if (nameTagComponents == NULL) {
        //log it as an error
        WriteError("Isometric World Control system failed to initialize: Scene does not have a 'name tag' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    isometricControlEntityIndex = ComponentNameTag_GetEntityIDFromEntityByName(nameTagComponents,"isometricControls",scn->numEntities);

    //if the entity with the isometric controls is missing
    if (isometricControlEntityIndex == -1) {
        //log it as an error
        WriteError("The scene does not have an entity with the name tag: 'isometricControls'. Add this entity and keyboard actions to it for controlling the isometric world.");
        //mark that the system has failed to initialize
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the key indexes
    keyScrollMapUp = componentInputKeyboardGetActionIndex(keyboardInputComponents,isometricControlEntityIndex,"up");
    keyScrollMapDown = componentInputKeyboardGetActionIndex(keyboardInputComponents,isometricControlEntityIndex,"down");
    keyScrollMapLeft = componentInputKeyboardGetActionIndex(keyboardInputComponents,isometricControlEntityIndex,"left");
    keyScrollMapRight = componentInputKeyboardGetActionIndex(keyboardInputComponents,isometricControlEntityIndex,"right");
    keyToggleGameMode = componentInputKeyboardGetActionIndex(keyboardInputComponents,isometricControlEntityIndex,"toggleGameMode");

    mouseWheelZoom = ComponentInputMouse_GetActionIndex(mouseInputComponents,isometricControlEntityIndex,"mouseWheel");
    mouseLeftClick = ComponentInputMouse_GetActionIndex(mouseInputComponents,isometricControlEntityIndex,"leftButton");

    //log that the isomeric world control system was successfully initialized
    WriteDebug("Initializing Isometric World Control System... DONE!");

    //return 1, successfully initialized the system
    return 1;
}

void SystemControlIsoWorld_Compute() {
    //if the system has failed to initialize
    if (systemFailedToInitialize==1) {
        //return out of the function
        return;
    }
    //if the component pointers have been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }
    //update the isometric mouse position
    IsoEngine_UpdateMousePos(isoEngine);

    if (isoEngine->gameMode == GAME_MODE_OVERVIEW) {
        //scroll the map when the mouse is close to the edges
        IsoEngine_ScrollMapWithMouse(isoEngine);
    }

    //if the mouse wheel is scrolling up
    if (mouseInputComponents[isometricControlEntityIndex].actions[mouseWheelZoom].state == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP) {
        IsoEngine_CenterMapToTileUnderMouse(isoEngine);
        IsoEngine_ZoomIn(isoEngine);
    }
    //if the mouse wheel is scrolling down
    if (mouseInputComponents[isometricControlEntityIndex].actions[mouseWheelZoom].state == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN) {
        IsoEngine_CenterMapToTileUnderMouse(isoEngine);
        IsoEngine_ZoomOut(isoEngine);
    }

    //if the left mouse button has just been pressed
    if (mouseInputComponents[isometricControlEntityIndex].actions[mouseLeftClick].state == COMPONENT_INPUTMOUSE_STATE_RELEASED
    && mouseInputComponents[isometricControlEntityIndex].actions[mouseLeftClick].oldState == COMPONENT_INPUTMOUSE_STATE_PRESSED) {
        IsoEngine_GetMouseTileClick(isoEngine);
    }

    //if the up key is pressed
    if (keyboardInputComponents[isometricControlEntityIndex].actions[keyScrollMapUp].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        isoEngine->mapScroll2Dpos.y+=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }

    //if the down key is pressed
    if (keyboardInputComponents[isometricControlEntityIndex].actions[keyScrollMapDown].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        isoEngine->mapScroll2Dpos.y-=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }

    //if the left key is pressed
    if (keyboardInputComponents[isometricControlEntityIndex].actions[keyScrollMapLeft].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        isoEngine->mapScroll2Dpos.x-=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }

    //if the right key is pressed
    if (keyboardInputComponents[isometricControlEntityIndex].actions[keyScrollMapRight].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        isoEngine->mapScroll2Dpos.x+=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }

    //if the toggle game mode key has just been pressed
    if (keyboardInputComponents[isometricControlEntityIndex].actions[keyToggleGameMode].oldState == COMPONENT_INPUTKEYBOARD_STATE_RELEASED
    && keyboardInputComponents[isometricControlEntityIndex].actions[keyToggleGameMode].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        isoEngine->gameMode++;
        if (isoEngine->gameMode>=NUM_GAME_MODES) {
            isoEngine->gameMode=GAME_MODE_OVERVIEW;
        }
    }
}

void SystemControlIsoWorld_Free() {
    //control isometric world system is not allocating anything, so we leave it empty
}
