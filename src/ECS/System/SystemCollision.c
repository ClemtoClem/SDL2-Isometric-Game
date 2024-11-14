#include <stdio.h>
#include "System.h"
#include "SystemCollision.h"
#include "../../logger.h"
#include "../../DeltaTimer.h"
#include "../Scene/Scene.h"
#include "../Components/Component.h"
#include "../../IsoEngine/isoEngine.h"
#include "SystemRenderIsoMetricWorld.h"

#define SYSTEM_COLLISION_MASK_SET1 (COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY | COMPONENT_SET1_COLLISION | COMPONENT_SET1_RENDER2D)

//local global functions
static void handleEntityWorldCollision(Uint32 entity);
static void handleEnityToEntityCollision(Uint32 entity);
static void checkPointCollision(Uint32 entity,int x,int y);
static void createWorldCollisionRect(Uint32 entity);

//local global pointer to the data
static ComponentPosition *posComponents = NULL;
static ComponentVelocity *velComponents = NULL;
static ComponentRender2D *renderComponents = NULL;
static ComponentCollision *colComponents = NULL;
static EntitiesOnScreen *onScreenEntities = NULL;



//local global pointer to the scene
static Scene *scn = NULL;

//local global pointer to the isoEngine
static IsoEngine *isoEngine = NULL;

//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers() {
    //if there is no scene
    if (scn == NULL) {
        //return out of the function
        return;
    }
    //Error handling is done in the scene.c file.

    //get the position component pointer
    posComponents = (ComponentPosition*)Scene_GetComponent(scn,COMPONENT_SET1_POSITION);

    //get the velocity component pointer
    velComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);

    //get the collision component pointer
    colComponents = (ComponentCollision*)Scene_GetComponent(scn,COMPONENT_SET1_COLLISION);

    //get the render 2D component pointer
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
}

int SystemCollision_Init(void *scene) {
    systemFailedToInitialize = 0;

    WriteDebug("Initializing collision system...");

    if (scene == NULL) {
        WriteError("Collision system failed to initialize: Parameter 'void *scene' is NULL ");
        systemFailedToInitialize = 1;
        return 0;
    }

    scn = (Scene*)scene;

    //if the passed scene is NULL
    if (scn->isoEngine == NULL) {
        WriteError("Collision system failed to initialize: The scene does not have the isometric engine! scene->isoEngine is NULL");
        systemFailedToInitialize = 1;
        return 0;
    }
    isoEngine = scn->isoEngine;
    //if the isoEngine is without a map
    if (isoEngine->isoMap == NULL) {
        WriteError("Collision system failed to initialize: The isoEngine does not have a map! isoEngine->isoMap is NULL");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has position components
    posComponents = (ComponentPosition*)Scene_GetComponent(scn,COMPONENT_SET1_POSITION);
    //if not
    if (posComponents == NULL) {
        //log the error
        WriteError("Collision system failed to initialize: The scene does not have position components (COMPONENT_SET1_POSITION)");
        systemFailedToInitialize = 1;
        return 0;
    }
    //check if the scene has velocity components
    velComponents = (ComponentVelocity*)Scene_GetComponent(scn,COMPONENT_SET1_VELOCITY);
    //if not
    if (velComponents == NULL) {
        //log the error
        WriteError("Collision system failed to initialize: The scene does not have velocity components (COMPONENT_SET1_VELOCITY)");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has collision components
    colComponents = (ComponentCollision*)Scene_GetComponent(scn,COMPONENT_SET1_COLLISION);
    //if not
    if (colComponents == NULL) {
        //log the error
        WriteError("Collision system failed to initialize: The scene does not have collision components (COMPONENT_SET1_COLLISION)");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has render 2D components
    renderComponents = (ComponentRender2D*)Scene_GetComponent(scn,COMPONENT_SET1_RENDER2D);
    //if not
    if (renderComponents == NULL) {
        //log the error
        WriteError("Collision system failed to initialize: The scene does not have render 2D components (COMPONENT_SET1_RENDER2D)");
        systemFailedToInitialize = 1;
        return 0;
    }
    WriteDebug("Initializing Collision System... DONE");
    //return 1, successfully initialized the system
    return 1;
}

void SystemCollision_Update() {
    //if there is no scene
    if (scn == NULL) {
        return;
    }

    //if the component pointers has been reallocated in the scene
    if (scn->componentPointersReallocated == 1) {
        updateComponentPointers();
    }
    onScreenEntities = SystemRenderIsoMetricWorld_GetEntitiesOnScreen(1);
}

void SystemCollision_UpdateEntity(Uint32 entity) {
    //if the system failed to initialize
    if (systemFailedToInitialize == 1) {
        return;
    }

    //if the entity has the position, velocity, render2D and collision component
    if (scn->entities[entity].componentSet1 & SYSTEM_COLLISION_MASK_SET1) {
        //reset is colliding to 0;
        colComponents[entity].isColliding = 0;
        //if collision detection is not active for the entity
        if (colComponents[entity].collisionType == COLLISIONTYPE_DEACTIVATED) {
            //exit out of the function
            return;
        }
        //if the entity can collide with the world
        if (colComponents[entity].collisionType == COLLISIONTYPE_WORLD
        || colComponents[entity].collisionType == COLLISIONTYPE_WORLD_AND_ENTITY) {
            handleEntityWorldCollision(entity);
        }
        if (colComponents[entity].collisionType == COLLISIONTYPE_ENTITY
        || colComponents[entity].collisionType == COLLISIONTYPE_WORLD_AND_ENTITY) {
            handleEnityToEntityCollision(entity);
        }
    }
}

static void handleEntityWorldCollision(Uint32 entity) {
    //check the bottom bottom rectangle points for the sprite collision
    checkPointCollision(entity,0,colComponents[entity].rect.h); //bottom left corner
    checkPointCollision(entity,colComponents[entity].rect.w,0); //bottom right corner
}
static void createWorldCollisionRect(Uint32 entity) {
    SDL_FPoint point;
    //get the entity world position
    point.x = (posComponents[entity].x*isoEngine->zoomLevel)+isoEngine->scrollX;
    point.y = (posComponents[entity].y*isoEngine->zoomLevel)+isoEngine->scrollY;
    IsoEngine_Convert2DToIso(&point);
    //apply the offset
    point.x += posComponents[entity].xOffset*isoEngine->zoomLevel;
    point.y += posComponents[entity].yOffset*isoEngine->zoomLevel;

    //create the collision rectangle
    //x,y start position for the rectanble
    //multiply with 0.5 to get the center of the texture
    colComponents[entity].worldRect.x = point.x +((renderComponents[entity].texture->cliprect.w*0.5)*isoEngine->zoomLevel)
                                                -((colComponents[entity].rect.w*0.5)*isoEngine->zoomLevel);
    //start at the bottom of the rectangle
    colComponents[entity].worldRect.y = point.y +((renderComponents[entity].texture->cliprect.h)*isoEngine->zoomLevel)
                                                -((colComponents[entity].rect.h)*isoEngine->zoomLevel);
    //width and height of the collision rectangle
    colComponents[entity].worldRect.w = colComponents[entity].rect.w*isoEngine->zoomLevel;
    colComponents[entity].worldRect.h = colComponents[entity].rect.h*isoEngine->zoomLevel;
}

static void handleEnityToEntityCollision(Uint32 entity) {
    Uint32 i = 0;

    createWorldCollisionRect(entity);
    for (i = 0; i < onScreenEntities->numEntitiesLastRender; ++i) {
        //if the entity is not it self
        if (onScreenEntities->entityList[i].entityID!=entity) {
            createWorldCollisionRect(onScreenEntities->entityList[i].entityID);

            //if there is a collision
            if (SystemCollision_BoundingBoxCollision(colComponents[entity].worldRect,colComponents[onScreenEntities->entityList[i].entityID].worldRect))
            {
                posComponents[entity].x = posComponents[entity].oldx[0];
                posComponents[entity].y = posComponents[entity].oldy[0];
                colComponents[entity].isColliding = 1;
            }
        }
    }
}

static void checkPointCollision(Uint32 entity,int x,int y) {
    SDL_FPoint point;
    int tile = 0;

    //check the width of the rectangle upwards
    point.x = (posComponents[entity].x + x)/isoEngine->isoMap->tileSize;
    point.y = (posComponents[entity].y + y)/isoEngine->isoMap->tileSize;

    //get the tile under the entity
    tile = isoMapGetTile(isoEngine->isoMap,point.x,point.y,renderComponents[entity].layer);

    //if the tile is valid
    if (tile!=-1) {
        //TODO: Add list of tiles that can be collided with
        if (tile == 2) {
            posComponents[entity].x = posComponents[entity].oldx[0];
            posComponents[entity].y = posComponents[entity].oldy[0];
            //mark that the entity is colliding
            colComponents[entity].isColliding = 1;
        }
    }

    //check the width of the rectangle downwards
    point.x = (posComponents[entity].x - x)/isoEngine->isoMap->tileSize;
    point.y = (posComponents[entity].y - y)/isoEngine->isoMap->tileSize;

    //get the tile under the entity
    tile = isoMapGetTile(isoEngine->isoMap,point.x,point.y,renderComponents[entity].layer);

    //if the tile is valid
    if (tile!=-1) {
        //TODO: Add list of tiles that can be collided with
        if (tile == 2) {
            posComponents[entity].x = posComponents[entity].oldx[0];
            posComponents[entity].y = posComponents[entity].oldy[0];
            //mark that the entity is colliding
            colComponents[entity].isColliding = 1;
        }
    }
}

int SystemCollision_BoundingBoxCollision(SDL_Rect a,SDL_Rect b) {
    //TODO: describe the collisions here
    if (b.x + b.w < a.x) return 0;
    if (b.x > a.x + a.w) return 0;
    if (b.y + b.h < a.y) return 0;
    if (b.y > a.y + a.h) return 0;
    return 1;
}

void SystemCollision_Free() {
    //nothing to free, the system is not allocating any memory
}
