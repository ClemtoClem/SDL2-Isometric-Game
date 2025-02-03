#ifndef __RENDER_ISOMETRIC_SYSTEM_H_
#define __RENDER_ISOMETRIC_SYSTEM_H_

//local struct for an on-screen entity
typedef struct EntityOnScreenPos {
    Uint32 entityID;        //entity ID to draw
    int cartesianYPos;      //entity Cartesian y position
    int row;                //which row the entity is standing on
} EntityOnScreenPos;

//Local struct to store entities that will be drawn on the screen
typedef struct EntitiesOnScreen {
    EntityOnScreenPos *entityList;     //list with entities on the screen
    Uint32 maxEntities;                 //current max entities on screen
    Uint32 numEntities;                 //number entities on screen
    Uint32 numEntitiesLastRender;       //number of entities last render call. Used by the collision system
    Uint32 currentEntityToDraw;         //current index in the sortedIndexList we want to draw
} EntitiesOnScreen;


int SystemRenderIsoMetricWorld_Init(void *scene);
void SystemRenderIsoMetricWorld_Compute();
void SystemRenderIsoMetricWorld_Free();
void SystemRenderIsoMetricWorld_SortEntity(Uint32 entity);
[[nodiscard]] EntitiesOnScreen *SystemRenderIsoMetricWorld_GetEntitiesOnScreen(int layer);

#endif // __RENDER_ISOMETRIC_SYSTEM_H_

