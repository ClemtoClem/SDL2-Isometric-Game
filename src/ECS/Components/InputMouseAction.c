#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "InputMouseAction.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentInputMouse *ComponentInputMouse_New() {
    int i = 0;
    ComponentInputMouse *newMouseComponent = malloc(sizeof(struct ComponentInputMouse)*NUM_INITIAL_ENTITIES);
    if (newMouseComponent == NULL) {
        WriteError("Could not allocate memory for mouse input component!");
        return NULL;
    }
    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        newMouseComponent[i].actions = NULL;        //set actions to NULL
        newMouseComponent[i].numActions = 0;        //number of actions
        newMouseComponent[i].maxActions = 0;        //current max actions
        newMouseComponent[i].active = 0;            //if this component is active
    }
    return newMouseComponent;
}

void ComponentInputMouse_AllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentInputMouse *newComponentInputMouse = realloc((ComponentInputMouse*)scene->components[componentIndex].data,sizeof(struct ComponentInputMouse)*scene->maxEntities);
    if (newComponentInputMouse == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for input mouse components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentInputMouse[j].actions = NULL;
        newComponentInputMouse[j].active = 0;
        newComponentInputMouse[j].maxActions = 0;
        newComponentInputMouse[j].numActions = 0;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentInputMouse;
}

void ComponentInputMouse_Free(ComponentInputMouse *componentInputMouse,int maxEntities) {
    int i = 0, j = 0;
    if (componentInputMouse !=NULL) {
        //if actions have been added
        if (componentInputMouse->actions!=NULL) {
            //loop through the current number of entities
            for (j = 0; j < maxEntities; ++j) {
                //loop through the actions
                for (i = 0; i < componentInputMouse[j].numActions; ++i) {
                    //if a name was given to an action
                    if (componentInputMouse[j].actions[i].name!=NULL) {
                        //free the allocated memory
                        free(componentInputMouse[j].actions[i].name);
                    }
                }
            }
            //free the actions
            free(componentInputMouse->actions);
        }
        //free the input mouse component
        free(componentInputMouse);
    }
}

void ComponentInputMouse_AddAction(ComponentInputMouse *componentInputMouse,int entity,char *name,InputMouseActionType mouseAction) {
    int actionIndex=-1;
    InputMouseAction *newMouseAction = NULL;
    if (componentInputMouse == NULL) {
        WriteError("Parameter: 'ComponentInputMouse *componentInputMouse' is NULL. Action:%s is not added!",name);
        return;
    }

    //The mouse functions differently from the
    //if the action already exist
    actionIndex = ComponentInputMouse_GetActionIndex(componentInputMouse,entity,name);
    if (actionIndex!=-1) {
        WriteError("Action:%s already exist, action is not added!",name);
        return;
    }

    int numActions = componentInputMouse[entity].numActions;

    //if memory needs to be allocated
    if (numActions >= componentInputMouse[entity].maxActions) {
        //if there are no actions
        if (componentInputMouse[entity].actions == NULL) {
            //allocate memory for five actions
            componentInputMouse[entity].maxActions += 5;
            componentInputMouse[entity].actions = malloc(sizeof(struct InputMouseAction)*componentInputMouse[entity].maxActions);
            if (componentInputMouse[entity].actions == NULL) {
                WriteError("Could not allocate memory for a new Action. Action:%s was not added to entity:%d!",name,entity);
                return;
            }
        }
        //if there are previous actions, re-allocate more memory
        else{
            //allocate memory for five more actions
            componentInputMouse[entity].maxActions += 5;
            newMouseAction = realloc(componentInputMouse[entity].actions,sizeof(struct InputMouseAction)*componentInputMouse[entity].maxActions);
            if (newMouseAction == NULL) {
                WriteError("Could not re-allocate more memory for mouse actions. Action:%s was not added!",name);
                return;
            }
            //point the actions to the new memory
            componentInputMouse[entity].actions = newMouseAction;
        }
    }

    //add the mouse action
    //allocate memory for the name
    componentInputMouse[entity].actions[numActions].name = strdup(name);
    //copy the name
    sprintf(componentInputMouse[entity].actions[numActions].name,"%s",name);
    //set the mouse action
    componentInputMouse[entity].actions[numActions].mouseAction = mouseAction;

    //set the mouse action states
    componentInputMouse[entity].actions[numActions].state = COMPONENT_INPUTMOUSE_STATE_RELEASED;
    componentInputMouse[entity].actions[numActions].oldState = COMPONENT_INPUTMOUSE_STATE_RELEASED;

    //Update number of actions
    componentInputMouse[entity].numActions++;

}

int ComponentInputMouse_GetActionIndex(ComponentInputMouse *componentInputMouse,Uint32 entity,char *actionName) {
    int i = 0;    //if the keyboard input component is valid
    if (componentInputMouse != NULL) {
        //loop through the input keyboard actions of the entity
        for (i = 0; i < componentInputMouse[entity].numActions; ++i) {
            //if the name matches
            if (strcmp(actionName,componentInputMouse[entity].actions[i].name)==0) {
                //return the index
                return i;
            }
        }
    }
    return -1;
}

void ComponentInputMouse_SetActiveState(ComponentInputMouse *componentInputMouse,Uint32 entity,int value) {
    if (componentInputMouse != NULL) {
        //if the value is 1 or 0
        if (value == 1 || value == 0) {
            componentInputMouse[entity].active = value;
        } else{
            WriteError("Cannot set state for entity:%d, parameter:'int value' must be 1 or 0. Invalid passed value was:%d",entity,value);
        }
    }
}

