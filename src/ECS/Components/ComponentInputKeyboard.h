#ifndef __COMPONENT_INPUT_KEYBOARD_H
#define __COMPONENT_INPUT_KEYBOARD_H

#include <SDL2/SDL.h>
#include "../../Timer.h"

#define COMPONENT_INPUTKEYBOARD_STATE_RELEASED  0
#define COMPONENT_INPUTKEYBOARD_STATE_PRESSED   1
#define NUM_OF_PREVIOUS_ACTIONS 4

//forward declaration of Scene, allows us to use the Scene struct without causing a cross-referencing header error
typedef struct Scene Scene;

typedef struct InputKeyboardAction {
    char *name;                     //name of the action
    SDL_Scancode scanCode;          //what key to map the action to
    char state;                     //current state of the key
    char oldState;                  //last key state
} InputKeyboardAction;

typedef struct ComponentInputKeyboard {
    int numActions;                 //number of mapped actions
    int maxActions;                 //current max number of actions
    InputKeyboardAction *actions;  //keyboard actions
    char active;                    //if the keyboard component is active
    int previousActions[NUM_OF_PREVIOUS_ACTIONS]; //which actions that was previously fired
    Timer actionRelease;           //how long to hold actions in the previous actions array
} ComponentInputKeyboard;

ComponentInputKeyboard *componentNewInputKeyboard();
void componentInputKeyboardAllocateMoreMemory(Scene *scene,int componentIndex);
void componentFreeInputKeyboard(ComponentInputKeyboard *componentInputKeyboard,int numEntities);
void ComponentInputKeyboard_AddAction(ComponentInputKeyboard *componentInputKeyboard,int entity,char *name,SDL_Scancode scanCode);
int componentInputKeyboardGetActionIndex(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity,char *actionName);
void ComponentInputKeyboard_SetActiveState(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity,int value);
void componentKeyboardUpdateActionReleaseTimer(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity);
void componentKeyboardInitActionReleaseTimer(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity);


#endif // __COMPONENT_INPUT_KEYBOARD_H


