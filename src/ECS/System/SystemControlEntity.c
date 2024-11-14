#include <stdio.h>
#include <string.h>
#include "System.h"
#include "SystemControlIsoWorld.h"
#include "../../logger.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"
#include "../../IsoEngine/isoEngine.h"

//define a mask for the control isometric system. It requires the keyboard component
//it works on SET1 components, so we mark that as well in the define
#define SYSTEM_CONTROL_ENTITY_MASK_SET1 (COMPONENT_SET1_KEYBOARD | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_VELOCITY)

//function prototypes
static void mapKeyboardControl(Scene *scene,int *key,char *action);
static void mapMouseControl(Scene *scene,int *mouseAction,char *action);
static void setKeysAndMouseControls(Scene *scene);

//local global variable for system failure
static int systemFailedToInitialize = 1;

//local global pointer to the components data
Scene *scn = NULL;
static ComponentInputKeyboard *keyboardInputComponents = NULL;
static ComponentInputMouse *mouseInputComponents = NULL;
static ComponentNameTag *nameTagComponents = NULL;
static ComponentVelocity *velocityComponents = NULL;
static ComponentRender2D *renderComponents = NULL;
static ComponentAnimation *animComponents = NULL;
static ComponentCollision *colComponents = NULL;

//list of the common keys
static int keyMoveUp = -1;
static int keyMoveDown = -1;
static int keyMoveLeft = -1;
static int keyMoveRight = -1;
static int mouseWheel = -1;
static int mouseLeftClick = -1;

//we assume that controllable entity ID:s will be less than: 2,147,483,647 (max signed int32 value)
static Sint32 selectedEntityToControl = -1;
static Sint32 playerEntityID = -1;

static void updateComponentPointers() {
    if (scn == NULL) {
        return;
    }
    //ERROR handling is done in the scene.c file for the components. If a realloc fail, the system will shut down there.

    //get the pointer to the keyboard input components
    keyboardInputComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);
    //get the mouse input component
    mouseInputComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);
    //get the velocity components
    velocityComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);
    //get the pointer to the name tag components
    nameTagComponents = (ComponentNameTag*)Scene_GetComponent(scn,COMPONENT_SET1_NAMETAG);
    //get the pointer to the render2D components
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
    //get the pointer to the animation components
    animComponents = (ComponentAnimation*)Scene_GetComponent(scn,COMPONENT_SET1_ANIMATION);
    //get the pointer to the animation components
    colComponents = (ComponentCollision*)Scene_GetComponent(scn,COMPONENT_SET1_COLLISION);
}

int SystemControlEntity_Init(void *scene) {
    systemFailedToInitialize=0;

    WriteDebug("Initializing Entity Control System...");
    //if the passed entity manager is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: *scene is NULL!");

        //mark that the system failed to initialize
        systemFailedToInitialize = 1;
        //exit the function
        return 0;
    }
    //typecast the void *scene to a Scene* pointer
    scn = (Scene*)scene;
    //get the pointer to the keyboard input components
    keyboardInputComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);
    //if the scene does not have a kayboard
    if (keyboardInputComponents == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: Scene does not have 'Keyboard input' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the mouse input component
    mouseInputComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);
    //if the scene does not have the mouse component
    if (mouseInputComponents == NULL) {
        //log it as an error
        WriteError("Entity World Control system failed to initialize: Scene does not have 'Mouse input' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the velocity components
    velocityComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);
    //if the scene does not have velocity components
    if (velocityComponents == NULL) {
        //log it as an error
        WriteError("Entity World Control system failed to initialize: Scene does not have 'velocity' component!");
        systemFailedToInitialize = 1;
        return 0;
    }
    //get the pointer to the name tag components
    nameTagComponents = (ComponentNameTag*)Scene_GetComponent(scn,COMPONENT_SET1_NAMETAG);
    if (nameTagComponents == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: Scene does not have a 'name tag' component!");
        systemFailedToInitialize = 1;
        return 0;
    }
    //get the pointer to the render2D components
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
    if (renderComponents == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: Scene does not have 'render2D' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the animation components
    animComponents = (ComponentAnimation*)Scene_GetComponent(scn,COMPONENT_SET1_ANIMATION);
    if (animComponents == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: Scene does not have 'animation' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the animation components
    colComponents = (ComponentCollision*)Scene_GetComponent(scn,COMPONENT_SET1_COLLISION);
    if (colComponents == NULL) {
        //log it as an error
        WriteError("Entity Control system failed to initialize: Scene does not have 'collision' component!");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the player ID (if it exist)
    playerEntityID = ComponentNameTag_GetEntityIDFromEntityByName(nameTagComponents,"player1",scn->numEntities);

    //log that the isomeric world control system was successfully initialized
    WriteDebug("Initializing Entity Control System... DONE!");

    //return 1, successfully initialized the system
    return 1;
}

void SystemControlEntity_Compute() {
    int controlledEntityIsPlayer1 = 0;
    SDL_Rect tmpRect;
    int isColliding = 0;

    //if the system has failed to initialize
    if (systemFailedToInitialize==1 || selectedEntityToControl==-1) {
        //return out of the function
        return;
    }
    //if the component pointers have been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }

    //if the entity being controlled is the player
    if (selectedEntityToControl == playerEntityID) {
        //flag that this is so
        controlledEntityIsPlayer1 = 1;
    }

    //if the mouse wheel is scrolling up
    if (mouseWheel !=-1 && mouseInputComponents[selectedEntityToControl].actions[mouseWheel].state == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP) {
        //do something
    }
    //if the mouse wheel is scrolling down
    if (mouseWheel !=-1 && mouseInputComponents[selectedEntityToControl].actions[mouseWheel].state == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN) {
        //do something
    }

    //if the left mouse button has just been pressed
    if (mouseLeftClick !=-1 && mouseInputComponents[selectedEntityToControl].actions[mouseLeftClick].state == COMPONENT_INPUTMOUSE_STATE_RELEASED
    && mouseInputComponents[selectedEntityToControl].actions[mouseLeftClick].oldState == COMPONENT_INPUTMOUSE_STATE_PRESSED) {
        //do something
    }

    //if the entity has a collision component
    if (scn->entities[selectedEntityToControl].componentSet1 & COMPONENT_SET1_COLLISION) {

        //if the entity is colliding
        if (colComponents[selectedEntityToControl].isColliding) {
            isColliding = 1;
        }
    }

    ///KEYBOARD CONTROLS

    //if action keys: right & down is pressed
    if (keyMoveRight !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveRight].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED
    && keyMoveDown !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveDown].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_DOWNRIGHT;
        velocityComponents[selectedEntityToControl].x = 100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkDownRight");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownRight");
        }
    }
    //if action keys: right & up is pressed
    else if (keyMoveRight !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveRight].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED
    && keyMoveUp !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveUp].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_UPRIGHT;
        velocityComponents[selectedEntityToControl].y = -100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkUpRight");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpRight");
        }
    }
    //if action keys: left & up is pressed
    else if (keyMoveLeft !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveLeft].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED
    && keyMoveUp !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveUp].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_UPLEFT;
        velocityComponents[selectedEntityToControl].x = -100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkUpLeft");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpLeft");
        }
    }
    //if action keys: left & down is pressed
    else if (keyMoveLeft !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveLeft].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED
    && keyMoveDown !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveDown].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_DOWNLEFT;
        velocityComponents[selectedEntityToControl].y = 100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkDownLeft");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownLeft");
        }
    }
    //if the up key is pressed
    else if (keyMoveUp !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveUp].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_UP;
        velocityComponents[selectedEntityToControl].x = -100;
        velocityComponents[selectedEntityToControl].y = -100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkUp");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUp");
        }
    }
    //if the down key is pressed
    else if (keyMoveDown !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveDown].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_DOWN;
        velocityComponents[selectedEntityToControl].x = 100;
        velocityComponents[selectedEntityToControl].y = 100;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkDown");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDown");
        }
    }
    //if the left key is pressed
    else if (keyMoveLeft !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveLeft].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_LEFT;
        velocityComponents[selectedEntityToControl].x = -50;
        velocityComponents[selectedEntityToControl].y = 50;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkLeft");
        }
        //if the entity has collided
        else {
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleLeft");
        }
    }
    //if the right key is pressed
    else if (keyMoveRight !=-1 && keyboardInputComponents[selectedEntityToControl].actions[keyMoveRight].state == COMPONENT_INPUTKEYBOARD_STATE_PRESSED) {
        animComponents[selectedEntityToControl].direction = ENTITY_WORLD_DIRECTION_RIGHT;
        velocityComponents[selectedEntityToControl].x = 50;
        velocityComponents[selectedEntityToControl].y = -50;
        if (controlledEntityIsPlayer1 && isColliding == 0) {
            //set the animation state for the direction
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"walkRight");
        }
        //if the entity has collided
        else{
            ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleRight");
        }
    } else {
        componentKeyboardInitActionReleaseTimer(keyboardInputComponents,selectedEntityToControl);
        if (controlledEntityIsPlayer1) {
            if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_UP) {
                if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveLeft) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpLeft");
                }
                else if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveRight) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpRight");
                } else {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUp");
                }
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_DOWN) {
                if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveLeft) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownLeft");
                } else if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveRight) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownRight");
                } else {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDown");
                }
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_LEFT) {
                if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveUp) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpLeft");
                } else if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveDown) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownLeft");
                } else {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleLeft");
                }
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_RIGHT) {
                if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveUp) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpRight");
                } else if (keyboardInputComponents[selectedEntityToControl].previousActions[1] == keyMoveDown) {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownRight");
                } else {
                    ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleRight");
                }
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_UPLEFT) {
                ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpLeft");
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_UPRIGHT) {
                ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleUpRight");
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_DOWNLEFT) {
                ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownLeft");
            } else if (animComponents[selectedEntityToControl].direction == ENTITY_WORLD_DIRECTION_DOWNRIGHT) {
                ComponentAnimation_SetAnimationState(animComponents,selectedEntityToControl,"idleDownRight");
            }
        }
    }
}

void SystemControlEntity_SetEntityToControlByNameTag(Scene *scene,char *nameTag) {
    Uint32 i = 0;
    if (scene == NULL) {
        //log it as an error
        WriteError("Parameter:'Scene *scene' is NULL!");
        return;
    }
    if (nameTag == NULL) {
        //log it as an error
        WriteError("Parameter:'char *nameTag' is NULL!");
        return;
    }

    //if the components have been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }

    //loop through all entities
    for (i = 0; i < scene->numEntities; ++i) {
        //if the entity with the name tag was found
        if (strcmp(nameTagComponents[i].name,nameTag)==0) {
            //set the index for the selected entity to i
            selectedEntityToControl = i;

            //set the keys and mouse controls
            setKeysAndMouseControls(scene);
            //return out of the function
            return;
        }
    }

    //If the entity was not found
    WriteWarning("Scene:%s, does not have an entity with the name tag:%s",scene->name,nameTag);
}

void SystemControlEntity_SetEntityToControlByID(Scene *scene,Uint32 entityID) {
    if (scene == NULL) {
        //log it as an error
        WriteError("Parameter:'Scene *scene' is NULL!");
        return;
    }
    if (entityID > scene->numEntities) {
        //log it as an error
        WriteError("entityID is > than number of entities");
        return;
    }
    //if the component pointers have been reallocated in the scene
    if (scene->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }

    //set the entity to control
    selectedEntityToControl = entityID;
    setKeysAndMouseControls(scene);
}

static void mapKeyboardControl(Scene *scene,int *key,char *action) {
    int writeErrorWithIndex = 0;

    ComponentInputKeyboard *keyboardInputComp = (ComponentInputKeyboard*)Scene_GetComponent(scene,COMPONENT_SET1_KEYBOARD);
    if (keyboardInputComp == NULL) {
        WriteError("Scene %s does not have the COMPONENT_SET1_KEYBOARD component!",scene->name);
    }

    ComponentNameTag *nameTagComp = (ComponentNameTag*)Scene_GetComponent(scene,COMPONENT_SET1_NAMETAG);
    if (nameTagComp == NULL) {
        writeErrorWithIndex = 1;
    }


    *key = componentInputKeyboardGetActionIndex(keyboardInputComp,selectedEntityToControl,action);
    if (*key == -1) {
        //log it as a warning
        if (writeErrorWithIndex==0) {
            WriteError("Key action: '%s' is not mapped for the entity:%s",action,nameTagComp[selectedEntityToControl].name);
        } else {
            WriteError("Key action: '%s' is not mapped for the entity:%d",action,selectedEntityToControl);
        }
    }
}

static void mapMouseControl(Scene *scene,int *mouseAction,char *action) {
    int writeErrorWithIndex = 0;
    ComponentInputMouse *inputMouseComp = (ComponentInputMouse*)Scene_GetComponent(scene,COMPONENT_SET1_MOUSE);
    if (inputMouseComp == NULL) {
        WriteError("Scene %s, does not have the COMPONENT_SET1_MOUSE component! Mouse actions not available!",scene->name);
        return;
    }
    ComponentNameTag *nameTagComp = (ComponentNameTag*)Scene_GetComponent(scene,COMPONENT_SET1_NAMETAG);
    if (nameTagComp == NULL) {
        writeErrorWithIndex = 1;
    }

    *mouseAction = ComponentInputMouse_GetActionIndex(mouseInputComponents,selectedEntityToControl,action);
    if (*mouseAction == -1) {
        //log it as a warning
        if (writeErrorWithIndex==0) {
            WriteError("Mouse action: '%s' is not mapped for the entity:%s",action,nameTagComp[selectedEntityToControl].name);
        } else {
            WriteError("Mouse action: '%s' is not mapped for the entity:%d",action,selectedEntityToControl);
        }
    }
}

static void setKeysAndMouseControls(Scene *scene) {
    //get the key indexes
    mapKeyboardControl(scene,&keyMoveUp,"up");
    mapKeyboardControl(scene,&keyMoveDown,"down");
    mapKeyboardControl(scene,&keyMoveLeft,"left");
    mapKeyboardControl(scene,&keyMoveRight,"right");

    //get the mouse actions
    mapMouseControl(scene,&mouseWheel,"mouseWheel");
    mapMouseControl(scene,&mouseLeftClick,"leftButton");
}

Uint32 SystemControlEntity_GetControlledEntity() {
    return selectedEntityToControl;
}

void SystemControlEntity_Free() {
    //control entity system is not allocating anything, so we leave it empty
}

