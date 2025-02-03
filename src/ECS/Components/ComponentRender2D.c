#include <stdlib.h>
#include <stdio.h>
#include "ComponentRender2D.h"
#include "ComponentPosition.h"
#include "../../logger.h"
#include "../Scene/Scene.h"

ComponentRender2D *ComponentRender2D_New() {
    Uint32 i = 0;    //allocate memory for new position components
    ComponentRender2D *newRenderComponent = malloc(sizeof(struct ComponentRender2D)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if (newRenderComponent==NULL) {
        //write the error to the logfile
        WriteError("Could not allocate memory for render components! ");
        return NULL;
    }

    //loop through all the entities
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        //set texture to NULL
        newRenderComponent[i].texture = NULL;
        newRenderComponent[i].layer = -1;
    }

    //return the pointer to the position components
    return newRenderComponent;
}

void ComponentRender2D_AllocateMoreMemory(Scene *scene,int componentIndex) {
    Uint32 j = 0;
    ComponentRender2D *newComponentRender2D = realloc((ComponentRender2D*)scene->components[componentIndex].data,sizeof(struct ComponentRender2D)*scene->maxEntities);
    if (newComponentRender2D == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for render2D components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentRender2D[j].texture = NULL;
        newComponentRender2D[j].layer = -1;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentRender2D;
}

void ComponentRender2D_Free(ComponentRender2D *render2Dcomponent) {
    //textures are not stored in the components themselves, therefore we will
    //not free the memory the texture pointer is pointing to.

    //if the position component is not NULL
    if (render2Dcomponent!=NULL) {
        //free it
        free(render2Dcomponent);
    }
}

void ComponentRender2D_SetTextureAndClipRect(ComponentRender2D *render2DComponent,Uint32 entity,Texture *texture,SDL_Rect *clipRect) {
    //if the render 2D component does not exist
    if (render2DComponent == NULL) {
        //write the error to the log file
        WriteError("Parameter: 'ComponentRender2D *render2DComponent' is NULL!  ");
        return;
    }

    //if the provided texture is NULL
    if (texture==NULL) {
        //write the error to the log file
        WriteError("Cannot set texture! Parameter: 'Texture *texture' is NULL!");
    }
    //set the texture
    render2DComponent[entity].texture = texture;

    //if the clipRect is NULL
    if (clipRect == NULL) {
        //set the whole texture as the clip rectangle
        SetupRect(&render2DComponent[entity].texture->cliprect,0,0,texture->width,texture->height);
    }
    else{
        //set the clip rectangle
        SetupRect(&render2DComponent[entity].texture->cliprect,clipRect->x,clipRect->y,clipRect->w,clipRect->h);
    }
}

void ComponentRender2D_SetClipRect(ComponentRender2D *render2DComponent,Uint32 entity,SDL_Rect *clipRect) {

    //if the render 2D component does not exist
    if (render2DComponent == NULL) {
        //write the error to the log file
        WriteError("Parameter: 'ComponentRender2D *render2DComponent' is NULL!  ");
        return;
    }
    if (render2DComponent[entity].texture == NULL) {
        //write the error to the log file
        WriteError("render2DComponent for entity:%d has no texture! render2DComponent is NULL!",entity);
        return;
    }
    //if the clipRect is NULL
    if (clipRect == NULL) {
        //set the whole texture as the clip rectangle
        SetupRect(&render2DComponent[entity].texture->cliprect,0,0,render2DComponent[entity].texture->width,render2DComponent[entity].texture->height);
    }
    else{
        //set the clip rectangle
        SetupRect(&render2DComponent[entity].texture->cliprect,clipRect->x,clipRect->y,clipRect->w,clipRect->h);
    }
}

void ComponentRender2D_SetLayer(ComponentRender2D *render2DComponent,Uint32 entity,int layer) {
    if (layer<0) {
        layer = 0;
    }
    render2DComponent[entity].layer = layer;
}
