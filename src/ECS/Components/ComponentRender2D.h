#ifndef __COMPONENT_RENDER2D_H
#define __COMPONENT_RENDER2D_H

#include "../../Texture.h"

//forward declaration of Scene, allows us to use the Scene struct without causing a cross-referencing header error
typedef struct Scene Scene;

typedef struct ComponentRender2D {
    Texture *texture;      //pointer to the texture
    int layer;              //which layer to render the component on
} ComponentRender2D;

ComponentRender2D *ComponentRender2D_New();
void ComponentRender2D_AllocateMoreMemory(Scene *scene,int componentIndex);
void ComponentRender2D_Free(ComponentRender2D *render2Dcomponent);
void ComponentRender2D_SetTextureAndClipRect(ComponentRender2D *render2DComponent,Uint32 entity,Texture *texture,SDL_Rect *clipRect);
void ComponentRender2D_SetClipRect(ComponentRender2D *render2DComponent,Uint32 entity,SDL_Rect *clipRect);
void ComponentRender2D_SetLayer(ComponentRender2D *render2DComponent,Uint32 entity,int layer);

#endif // __COMPONENT_RENDER_H
