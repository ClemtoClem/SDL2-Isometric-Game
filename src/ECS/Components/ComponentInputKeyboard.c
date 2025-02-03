#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ComponentInputKeyboard.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentInputKeyboard *componentNewInputKeyboard() {
    int i = 0;    ComponentInputKeyboard *newKeyboardComponent = malloc(sizeof(struct ComponentInputKeyboard)*NUM_INITIAL_ENTITIES);
    if (newKeyboardComponent == NULL) {
        WriteError("Could not allocate memory for keyboard input component!");
        return NULL;
    }
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        newKeyboardComponent[i].actions = NULL;     //set actions to NULL
        newKeyboardComponent[i].numActions = 0;     //number of actions
        newKeyboardComponent[i].maxActions = 0;     //current max number of actions
        newKeyboardComponent[i].active = 0;         //the component state is set to not active
        for (int j = 0; j < NUM_OF_PREVIOUS_ACTIONS; j++) {
            newKeyboardComponent[i].previousActions[j] = -1; //previous keyboard action
        }
        Timer_Init(&newKeyboardComponent[i].actionRelease,150);
    }

    return newKeyboardComponent;
}

void componentInputKeyboardAllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentInputKeyboard *newComponentInputKeyboard = realloc((ComponentInputKeyboard*)scene->components[componentIndex].data,
                                                                 sizeof(struct ComponentInputKeyboard)*scene->maxEntities);
    if (newComponentInputKeyboard == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for input keyboard components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }

    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentInputKeyboard[j].actions = NULL;
        newComponentInputKeyboard[j].active = 0;
        newComponentInputKeyboard[j].maxActions = 0;
        newComponentInputKeyboard[j].numActions = 0;
        for (int k = 0; k < NUM_OF_PREVIOUS_ACTIONS; k++) {
            newComponentInputKeyboard[j].previousActions[k] = -1; //previous keyboard action
        }
        Timer_Init(&newComponentInputKeyboard[j].actionRelease,150);
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentInputKeyboard;
}

void componentInputKeyboardAddAction(ComponentInputKeyboard *componentInputKeyboard,int entity,char *name,SDL_Scancode scanCode) {
    int actionIndex=-1;
    InputKeyboardAction *newKeyboardAction = NULL;
    if (componentInputKeyboard == NULL) {
        WriteError("Parameter: componentInputKeyboard is NULL. Action:%s is not added!",name);
        return;
    }
    //if the action already exist
    actionIndex = componentInputKeyboardGetActionIndex(componentInputKeyboard,entity,name);
    if (actionIndex!=-1) {
        //change the key scan code for the action
        componentInputKeyboard[entity].actions[actionIndex].scanCode = scanCode;
        //return out of the function
        return;
    }

    //if memory needs to be allocated
    if (componentInputKeyboard[entity].numActions >= componentInputKeyboard[entity].maxActions) {
        //there are no actions
        if (componentInputKeyboard[entity].actions == NULL) {
            //allocate memory for five actions
            componentInputKeyboard[entity].maxActions+=5;
            componentInputKeyboard[entity].actions = malloc(sizeof(struct InputKeyboardAction)*componentInputKeyboard[entity].maxActions);
            if (componentInputKeyboard[entity].actions == NULL) {
                WriteError("Could not allocate memory for a new Action. Action:%s was not added to entity:%d!",name,entity);
                return;
            }
        }
        //if there are previous actions, re-allocate more memory
        else{
            //allocate memory for five more actions
            componentInputKeyboard[entity].maxActions+=5;
            newKeyboardAction = realloc(componentInputKeyboard[entity].actions,sizeof(struct InputKeyboardAction)*componentInputKeyboard[entity].maxActions);
            if (newKeyboardAction == NULL) {
                WriteError("Could not re-allocate more memory for keyboard actions. Action:%s was not added!",name);
                return;
            }
            //point the actions to the new memory
            componentInputKeyboard[entity].actions = newKeyboardAction;
        }
    }

    //add the keyboard action
    //allocate memory for the name
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].name = malloc(sizeof(char)*strlen(name)+1);
    //copy the name
    sprintf(componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].name,"%s",name);
    //set the keyboard scan code
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].scanCode = scanCode;

    //set the key states
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].state = COMPONENT_INPUTKEYBOARD_STATE_RELEASED;
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].oldState = COMPONENT_INPUTKEYBOARD_STATE_RELEASED;

    //Update number of actions
    componentInputKeyboard[entity].numActions++;

}

int componentInputKeyboardGetActionIndex(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity,char *actionName) {
    int i = 0;    //if the keyboard input component is valid
    if (componentInputKeyboard != NULL) {
        //loop through the input keyboard actions of the entity
        for (i = 0; i < componentInputKeyboard[entity].numActions; ++i) {
            //if the name matches
            if (strcmp(actionName,componentInputKeyboard[entity].actions[i].name)==0) {
                //return the index
                return i;
            }
        }
    }
    return -1;
}

void componentFreeInputKeyboard(ComponentInputKeyboard *componentInputKeyboard,int maxEntities) {
    int i = 0, j = 0;
    if (componentInputKeyboard !=NULL) {
        //loop through all entities
        for (j = 0; j < maxEntities; ++j) {
            //if actions have been added
            if (componentInputKeyboard[j].actions!=NULL)
            {
                //loop through the actions
                for (i = 0; i < componentInputKeyboard[j].numActions; ++i)
                {
                    //if a name was given to an action
                    if (componentInputKeyboard[j].actions[i].name!=NULL)
                    {
                        //free the allocated memory
                        free(componentInputKeyboard[j].actions[i].name);
                    }
                }
            }
            //free the actions
            free(componentInputKeyboard[j].actions);
        }
        //free the input keyboard component
        free(componentInputKeyboard);
    }
}

void componentInputKeyboardSetActiveState(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity,int value) {
    if (componentInputKeyboard !=NULL) {
        //if the value is 1 or 0
        if (value==1 || value == 0) {
            componentInputKeyboard[entity].active = value;
        }
        else{
            WriteError("Cannot set state for entity:%d, parameter:int value must be 1 or 0. Invalid passed value was:%d",entity,value);
        }
    }
}
void componentKeyboardInitActionReleaseTimer(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity) {
    if (componentInputKeyboard !=NULL) {
        Timer_Init(&componentInputKeyboard[entity].actionRelease,150);
    }
}
void componentKeyboardUpdateActionReleaseTimer(ComponentInputKeyboard *componentInputKeyboard,Uint32 entity) {
    if (componentInputKeyboard !=NULL) {
        if (Timer_Update(&componentInputKeyboard[entity].actionRelease)) {
            // switch the previous action to the current action
            for (int i = 0; i < NUM_OF_PREVIOUS_ACTIONS-1; i++) {
                componentInputKeyboard[entity].previousActions[i+1] = componentInputKeyboard[entity].previousActions[i];
            }
            componentInputKeyboard[entity].previousActions[0] = -1;
        }
    }
}
