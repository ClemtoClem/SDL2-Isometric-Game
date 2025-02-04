#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ComponentWidget.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentWidget *ComponentWidget_New() {
    int i = 0;
    ComponentWidget *newWidgetComponent = malloc(sizeof(struct ComponentInputMouse)*NUM_INITIAL_ENTITIES);
    if (newWidgetComponent == NULL) {
        WriteError("Could not allocate memory for widget component!");
        return NULL;
    }
    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        newWidgetComponent[i].state = COMPONENT_WIDGET_STATE_IDLE;
        newWidgetComponent[i].nbOfChilds = 0;
    }
    return newWidgetComponent;
}

void ComponentWidget_AllocMoreMemory(Scene *scene, int componentIndex) {
    Uint32 j = 0;
    ComponentWidget *newComponentWidget = realloc((ComponentWidget*) scene->components[componentIndex].data, sizeof(struct ComponentWidget)*scene->maxEntities);
    if (newComponentWidget == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for widget components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        newWidgetComponent[i].state = COMPONENT_WIDGET_STATE_IDLE;
        newWidgetComponent[i].nbOfChilds = 0;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newWidgetComponent;
}

void ComponentWidget_Free(ComponentWidget *componentWidget, int numEntities) {
    int i = 0, j = 0;
    if (componentWidget !=NULL) {
        //if child have been added
        if (componentWidget->nbOfChilds!=0) {
            free(componentWidget->childs);
        }
        //free the input mouse component
        free(nbOfChilds);
    }
}
