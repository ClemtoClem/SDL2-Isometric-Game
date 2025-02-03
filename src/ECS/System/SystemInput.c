#include <SDL2/SDL.h>
#include <stdio.h>
#include "System.h"
#include "SystemInput.h"
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"

//define a masks for the input system. It requires either a keyboard or mouse component.
//it works on SET1 components, so we mark that as well in the define
#define SYSTEM_INPUT_KEYBOARD_MASK_SET1 (COMPONENT_SET1_KEYBOARD)
#define SYSTEM_INPUT_MOUSE_MASK_SET1 (COMPONENT_SET1_MOUSE)

#define COMPONENT_NO_INDEX -100

//local global variable for system failure
static int systemFailedToInitialize = 1;

//scene pointer
static Scene *scn = NULL;

//SDL event
static SDL_Event event;

//key states
static const Uint8* keyStates;

//mouse button states
static int mouseButtonLeftState         = COMPONENT_INPUTMOUSE_STATE_RELEASED;
static int mouseButtonRightState        = COMPONENT_INPUTMOUSE_STATE_RELEASED;
static int mouseButtonMiddleState       = COMPONENT_INPUTMOUSE_STATE_RELEASED;
static int mouseWheelState              = COMPONENT_INPUTMOUSE_STATE_MOUSE_WHEEL_NONE;

//pointer to the keyboard components
ComponentInputKeyboard *keyboardComponents = NULL;

//pointer to the mouse components
ComponentInputMouse *mouseComponents = NULL;

static void updateComponentPointers() {
    if (scn == NULL) {
        return;
    }
    //ERROR handling is done in the scene.c file for the components. If a realloc fail, the system will shut down there.

    //get the pointer to the keyboard components
    keyboardComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);
    //get the pointer to the keyboard components
    mouseComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);
}

int SystemInput_Init(void *scene) {
    systemFailedToInitialize=0;

    WriteDebug("Initializing Input system...");
    //if the passed entity manager is NULL
    if (scene == NULL) {
        //log it as an error
        WriteError("Input system failed to initialize: *scene is NULL!");

        //mark that the system failed to initialize
        systemFailedToInitialize = 1;
        //exit the function
        return 0;
    }
    //typecast the void *scene to a Scene* pointer
    scn = (Scene*)scene;

    //get the pointer to the keyboard components
    keyboardComponents = (ComponentInputKeyboard*)Scene_GetComponent(scn,COMPONENT_SET1_KEYBOARD);

    //if the scene does not have a keyboard component
    if (keyboardComponents == NULL) {
        //log it as an error
        WriteError("Input system failed to initialize: Scene does not have a COMPONENT_SET1_KEYBOARD");
        systemFailedToInitialize = 1;
        return 0;
    }

    //get the pointer to the keyboard components
    mouseComponents = (ComponentInputMouse*)Scene_GetComponent(scn,COMPONENT_SET1_MOUSE);

    //if the scene does not have a keyboard component
    if (mouseComponents == NULL) {
        //log it as a warning
        WriteWarning("Scene does not have a COMPONENT_SET1_MOUSE");
    }

    //flag that the initialization went ok
    systemFailedToInitialize = 0;

    //log that the Input system was successfully initialized
    WriteDebug("Initializing Input system... DONE!");

    //return 1, successfully initialized the system
    return 1;
}

void SystemInput_Update() {
    //if the component pointers have been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        //update the local global component pointers
        updateComponentPointers();
    }
    //update the current key states
    keyStates = SDL_GetKeyboardState(NULL);

    //reset the mouse wheel state to none
    mouseWheelState = COMPONENT_INPUTMOUSE_STATE_MOUSE_WHEEL_NONE;

    //Poll events
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                //handle the quit event here
                scn->exitScene = 1;
            break;

            case SDL_KEYUP:
                //if the user released the escape key
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    scn->exitScene = 1;
                }
            break;

            //mouse button down events
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    //set the mouse button left state to pressed
                    mouseButtonLeftState = COMPONENT_INPUTMOUSE_STATE_PRESSED;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    //set the mouse button middle state to pressed
                    mouseButtonMiddleState = COMPONENT_INPUTMOUSE_STATE_PRESSED;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    //set the mouse button right state to pressed
                    mouseButtonRightState = COMPONENT_INPUTMOUSE_STATE_PRESSED;
                }
            break;

            //mouse button up events
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    //set the mouse button middle state to released
                    mouseButtonLeftState = COMPONENT_INPUTMOUSE_STATE_RELEASED;
                }
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    //set the mouse button middle state to released
                    mouseButtonMiddleState = COMPONENT_INPUTMOUSE_STATE_RELEASED;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    //set the mouse button right state to released
                    mouseButtonRightState = COMPONENT_INPUTMOUSE_STATE_RELEASED;
                }
            break;

            //mouse wheel events
            case SDL_MOUSEWHEEL:
                //If the user scrolled the mouse wheel up
                if (event.wheel.y>=1)
                {
                    mouseWheelState = COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP;
                }
                //If the user scrolled the mouse wheel down
                else{
                    mouseWheelState = COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN;
                }
            break;
        }
    }
}

void SystemInput_UpdateEntity(Uint32 entity) {
    int i = 0;
    //if the Input system failed to initialize
    if (systemFailedToInitialize == 1) {
        //return out of the function
        return;
    }

    //if the entity has a mouse component and the scene has a mouse component
    if (scn->entities[entity].componentSet1 & SYSTEM_INPUT_MOUSE_MASK_SET1 && mouseComponents!=NULL) {

        //if the component is active
        if (mouseComponents[entity].active != 0) {
            //loop through the mouse actions the entity has
            for (i = 0; i < mouseComponents[entity].numActions; ++i) {
                //Get the old state for the actions, and set the new one
                if (mouseComponents[entity].actions[i].mouseAction == COMPONENT_INPUTMOUSE_ACTION_LEFTBUTTON) {
                    mouseComponents[entity].actions[i].oldState = mouseComponents[entity].actions[i].state;
                    mouseComponents[entity].actions[i].state = mouseButtonLeftState;

                    /* // uncomment to test
                    if (mouseComponents[entity].actions[i].oldState != mouseComponents[entity].actions[i].state)
                    {
                        if (mouseComponents[entity].actions[i].state == COMPONENT_INPUTMOUSE_STATE_PRESSED) {
                            WriteDebug("Left Mouse button is pressed:%s",mouseComponents[entity].actions[i].name);
                        }
                    }*/
                }
                if (mouseComponents[entity].actions[i].mouseAction == COMPONENT_INPUTMOUSE_ACTION_MIDDLEBUTTON) {
                    mouseComponents[entity].actions[i].oldState = mouseComponents[entity].actions[i].state;
                    mouseComponents[entity].actions[i].state = mouseButtonMiddleState;
                    /*  //uncomment to test
                    if (mouseComponents[entity].actions[i].oldState != mouseComponents[entity].actions[i].state)
                    {
                        if (mouseComponents[entity].actions[i].state == COMPONENT_INPUTMOUSE_STATE_PRESSED) {
                            WriteDebug("Middle Mouse button is pressed:%s",mouseComponents[entity].actions[i].name);
                        }
                    }*/
                }
                if (mouseComponents[entity].actions[i].mouseAction == COMPONENT_INPUTMOUSE_ACTION_RIGHTBUTTON) {
                    mouseComponents[entity].actions[i].oldState = mouseComponents[entity].actions[i].state;
                    mouseComponents[entity].actions[i].state = mouseButtonRightState;
                    /* //uncomment to test
                    if (mouseComponents[entity].actions[i].oldState != mouseComponents[entity].actions[i].state)
                    {
                        if (mouseComponents[entity].actions[i].state == COMPONENT_INPUTMOUSE_STATE_PRESSED) {
                            WriteDebug("Right Mouse button is pressed:%s",mouseComponents[entity].actions[i].name);
                        }
                    }*/
                }
                if (mouseComponents[entity].actions[i].mouseAction == COMPONENT_INPUTMOUSE_ACTION_MOUSEWHEEL) {
                    mouseComponents[entity].actions[i].oldState = mouseComponents[entity].actions[i].state;
                    mouseComponents[entity].actions[i].state = mouseWheelState;

                    /* //uncomment to test
                    if (mouseWheelState == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP) {
                        WriteDebug("Mouse action: Mouse wheel up! State:%d",keyboardComponents[entity].actions[i].state);
                    }
                    if (mouseWheelState == COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN) {
                        WriteDebug("Mouse action: Mouse wheel Down! State:%d",keyboardComponents[entity].actions[i].state);
                    }*/
                }
            }
        }
    }

    //if the entity has a keyboard component
    if (scn->entities[entity].componentSet1 & SYSTEM_INPUT_KEYBOARD_MASK_SET1) {
        //if the component is active
        if (keyboardComponents[entity].active != 0) {
            //update the action release timer
            componentKeyboardUpdateActionReleaseTimer(keyboardComponents,entity);

            //loop through the keyboard actions the entity has
            for (i = 0; i < keyboardComponents[entity].numActions; ++i) {
                //if the mapped scan code is pressed
                if (keyStates[keyboardComponents[entity].actions[i].scanCode])
                {
                    //set old state
                    keyboardComponents[entity].actions[i].oldState = keyboardComponents[entity].actions[i].state;
                    //set state to pressed
                    keyboardComponents[entity].actions[i].state = COMPONENT_INPUTKEYBOARD_STATE_PRESSED;
                    if (keyboardComponents[entity].actions[i].name!=NULL) {
                        //uncomment the line to test
                        //WriteDebug("Key pressed:%s",keyboardComponents[entity].actions[i].name);
                    }
                }
                //if it's not pressed
                else{
                    //set old state
                    keyboardComponents[entity].actions[i].oldState = keyboardComponents[entity].actions[i].state;
                    //reset state to released
                    keyboardComponents[entity].actions[i].state = COMPONENT_INPUTKEYBOARD_STATE_RELEASED;
                    //if the old state was pressed, and the new state is released
                    if (keyboardComponents[entity].actions[i].oldState == COMPONENT_INPUTKEYBOARD_STATE_PRESSED
                    && keyboardComponents[entity].actions[i].state == COMPONENT_INPUTKEYBOARD_STATE_RELEASED)
                    {
                        if (keyboardComponents[entity].actions[i].name!=NULL) {
                            //uncomment the line to test
                            WriteDebug("Key released:%s - previous action index:%d",keyboardComponents[entity].actions[i].name,i);
                            //set previous keyboard action
                            for (int j = 0; j < NUM_OF_PREVIOUS_ACTIONS-1; ++j) {
                                keyboardComponents[entity].previousActions[j+1] = keyboardComponents[entity].previousActions[j];
                            }
                            keyboardComponents[entity].previousActions[0] = i;
                        }
                    }
                }
            }
        }
    }
}

void SystemInput_Free() {
    //Input system is not allocating anything, so we leave it empty
}
