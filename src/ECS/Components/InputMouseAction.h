#ifndef __COMPONENT_INPUT_MOUSE_H
#define __COMPONENT_INPUT_MOUSE_H

#include <SDL2/SDL.h>

#define COMPONENT_INPUTMOUSE_STATE_RELEASED         0
#define COMPONENT_INPUTMOUSE_STATE_JUST_PRESSED     1
#define COMPONENT_INPUTMOUSE_STATE_PRESSED          2
#define COMPONENT_INPUTMOUSE_STATE_JUST_RELEASED    3
#define COMPONENT_INPUTMOUSE_STATE_MOUSE_WHEEL_NONE 0
#define COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP    1
#define COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN  2

//forward declaration of Scene, allows us to use the Scene struct without causing a cross-referencing header error
typedef struct Scene Scene;

//Enum of mouse input actions
typedef enum InputMouseActionType {
    COMPONENT_INPUTMOUSE_ACTION_LEFTBUTTON      = 0,
    COMPONENT_INPUTMOUSE_ACTION_MIDDLEBUTTON    = 1,
    COMPONENT_INPUTMOUSE_ACTION_RIGHTBUTTON     = 2,
    COMPONENT_INPUTMOUSE_ACTION_MOUSEWHEEL      = 3,
} InputMouseActionType;

typedef struct InputMouseAction {
    char *name;                     //name of the action
    char state;                     //current state of the action
    char oldState;                  //last state
    InputMouseActionType mouseAction;  //the mouse action
} InputMouseAction;

typedef struct ComponentInputMouse {
    int numActions;                 //number of mouse actions
    int maxActions;                 //max number of mouse actions
    InputMouseAction *actions;      //pointer to the mouse actions
    char active;                    //if the mouse component is active
} ComponentInputMouse;

[[nodiscard]] ComponentInputMouse *ComponentInputMouse_New();
void ComponentInputMouse_AllocateMoreMemory(Scene *scene, int componentIndex);
void ComponentInputMouse_Free(ComponentInputMouse *componentInputMouse, int numEntities);
void ComponentInputMouse_SetActiveState(ComponentInputMouse *componentInputMouse, Uint32 entity, int value);
void ComponentInputMouse_AddAction(ComponentInputMouse *componentInputMouse, int entity, char *name, InputMouseActionType mouseAction);
int ComponentInputMouse_GetActionIndex(ComponentInputMouse *componentInputMouse, Uint32 entity, char *actionName);

#endif // __COMPONENT_INPUT_MOUSE_H



