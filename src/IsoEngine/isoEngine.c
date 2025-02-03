#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "isoEngine.h"
#include "../logger.h"
#include "../DeltaTimer.h"
#include "../renderer.h"

IsoEngine *IsoEngine_New() {
    IsoEngine *isoEngine = malloc(sizeof(struct IsoEngine));

    if (isoEngine == NULL) {
        WriteError("Could not allocate memory for new isoEngine!");
        return NULL;
    }
    isoEngine->scrollX = 0;
    isoEngine->scrollY = 0;
    isoEngine->mapScrollSpeed = 1200;
    isoEngine->mapScroll2Dpos.x = 0;
    isoEngine->mapScroll2Dpos.y = 0;
    isoEngine->zoomLevel = 1.0;
    isoEngine->lastTileClicked = -1;
    isoEngine->isoMap = NULL;
    isoEngine->gameMode = GAME_MODE_OVERVIEW;

    SetupRect(&isoEngine->mouseRect,0,0,1,1);
    isoEngine->tilePos.x = 0;
    isoEngine->tilePos.y = 0;

    return isoEngine;
}

void IsoEngine_Init(IsoEngine *isoEngine, int tileSizeInPixels) {
    (void)tileSizeInPixels;
    (void)isoEngine;
}

void IsoEngine_Free(IsoEngine *isoEngine) {
    if (isoEngine != NULL) {
        if (isoEngine->isoMap!=NULL) {
            isoMapFreeMap(isoEngine->isoMap);
        }
        free(isoEngine);
    }
}

void IsoEngine_SetMapSize(IsoEngine *isoEngine, int width, int height) {
    (void)isoEngine;
    (void)width;
    (void)height;
}

void IsoEngine_Convert2DToIso(SDL_FPoint *point)
{
    int tmpX = point->x - point->y;
    int tmpY = (point->x + point->y)*0.5;
    point->x = tmpX;
    point->y = tmpY;
}

void IsoEngine_ConvertIsoTo2D(SDL_FPoint *point) {
    int tmpX = (2 * point->y + point->x)*0.5;
    int tmpY = (2 * point->y - point->x)*0.5;
    point->x = tmpX;
    point->y = tmpY;
}

void IsoEngine_GetTileCoordinates(IsoEngine *isoEngine,SDL_FPoint *point,SDL_FPoint *point2DCoord) {
    if (isoEngine == NULL) {
        WriteError("isoEngine is NULL!");
        return;
    }
    if (isoEngine->isoMap == NULL) {
        WriteError("isoEngine->isoMap is NULL!");
        return;
    }
    float tempX = (float)point->x / (float)isoEngine->isoMap->tileSize;
    float tempY = (float)point->y / (float)isoEngine->isoMap->tileSize;;

    //convert back to integer
    point2DCoord->x = (int)tempX;
    point2DCoord->y = (int)tempY;
}

void IsoEngine_ConvertIsoCameraToCartesian(IsoEngine *isoEngine,SDL_FPoint *cartesianCamPos) {
    SDL_FPoint tmpPoint;

    if (isoEngine == NULL || cartesianCamPos == NULL) {
        return;
    }

    tmpPoint.x = isoEngine->scrollX;
    tmpPoint.y = isoEngine->scrollY;

    IsoEngine_Convert2DToIso(&tmpPoint);

    tmpPoint.x = tmpPoint.x/2;

    if (tmpPoint.x<0) {
        tmpPoint.x = fabsf(tmpPoint.x);
    }
    else if (tmpPoint.x>0) {
        tmpPoint.x = -fabsf(tmpPoint.x);
    }
    *cartesianCamPos = tmpPoint;
}

void IsoEngine_ConvertIsoPoint2DToCartesian(IsoEngine *isoEngine,SDL_FPoint *isoPoint,SDL_FPoint *cartesianPoint) {
    SDL_FPoint tmpPoint;

    if (isoEngine == NULL || cartesianPoint == NULL || isoPoint == NULL) {
        return;
    }

    //copy the isometric point, we don't want to change the original
    tmpPoint = *isoPoint;

    //convert the point to isometric
    IsoEngine_Convert2DToIso(&tmpPoint);

    tmpPoint.x = tmpPoint.x*0.5; // same as tmpPoint.x/2;

    if (tmpPoint.x<0) {
        tmpPoint.x = fabsf(tmpPoint.x);
    }
    else if (tmpPoint.x>0) {
        tmpPoint.x = -fabsf(tmpPoint.x);
    }
    *cartesianPoint = tmpPoint;
}

void IsoEngine_ConvertCartesianCameraToIsometric(IsoEngine *isoEngine,SDL_FPoint *cartesianCamPos) {
    SDL_FPoint tmpPoint;

    if (isoEngine == NULL || cartesianCamPos == NULL) {
        return;
    }
    tmpPoint = *cartesianCamPos;

    tmpPoint.x = (int)tmpPoint.x*2;

    if (tmpPoint.x < 0) {
        tmpPoint.x = abs((int)tmpPoint.x);
    } else if (tmpPoint.x > 0) {
        tmpPoint.x = -abs((int)tmpPoint.x);
    }

    IsoEngine_ConvertIsoTo2D(&tmpPoint);

    isoEngine->scrollX = (int)tmpPoint.x;
    isoEngine->scrollY = (int)tmpPoint.y;
}

void IsoEngine_UpdateMousePos(IsoEngine *isoEngine) {
    SDL_GetMouseState(&isoEngine->mouseRect.x,&isoEngine->mouseRect.y);
    isoEngine->mouseRect.x = isoEngine->mouseRect.x/isoEngine->zoomLevel;
    isoEngine->mouseRect.y = isoEngine->mouseRect.y/isoEngine->zoomLevel;
}

void IsoEngine_ScrollMapWithMouse(IsoEngine *isoEngine) {
    int zoomEdgeX = (WINDOW_WIDTH*isoEngine->zoomLevel)-(WINDOW_WIDTH);
    int zoomEdgeY = (WINDOW_HEIGHT*isoEngine->zoomLevel)-(WINDOW_HEIGHT);

    if (isoEngine->mouseRect.x<2) {
        isoEngine->mapScroll2Dpos.x-=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }
    if (isoEngine->mouseRect.x>WINDOW_WIDTH-(zoomEdgeX/isoEngine->zoomLevel)-2) {
        isoEngine->mapScroll2Dpos.x+=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);

    }
    if (isoEngine->mouseRect.y<2) {
        isoEngine->mapScroll2Dpos.y+=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);

    }
    if (isoEngine->mouseRect.y>WINDOW_HEIGHT-(zoomEdgeY/isoEngine->zoomLevel)-2) {
        isoEngine->mapScroll2Dpos.y-=isoEngine->mapScrollSpeed*DeltaTimer_GetDeltaTime();
        IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
    }
}

void IsoEngine_DrawIsoMouse(IsoEngine *isoEngine) {
    if (isoEngine == NULL) {
        WriteError("isoEngine is NULL!");
        return;
    }
    if (isoEngine->isoMap == NULL) {
        WriteError("isoEngine->isoMap is NULL!");
        return;
    }
    int modulusX = isoEngine->isoMap->tileSize*isoEngine->zoomLevel;
    int modulusY = isoEngine->isoMap->tileSize*isoEngine->zoomLevel;
    int correctX =(((int)isoEngine->mapScroll2Dpos.x)%modulusX)*2;
    int correctY = ((int)isoEngine->mapScroll2Dpos.y)%modulusY;

    isoEngine->mousePoint.x = (isoEngine->mouseRect.x/isoEngine->isoMap->tileSize) * isoEngine->isoMap->tileSize;
    isoEngine->mousePoint.y = (isoEngine->mouseRect.y/isoEngine->isoMap->tileSize) * isoEngine->isoMap->tileSize;

    //For every other x position on the map
    if (((int)isoEngine->mousePoint.x/isoEngine->isoMap->tileSize)%2) {
        //Move the mouse down by half a tile so we can
        //pick isometric tiles on that row as well.
        isoEngine->mousePoint.y+=isoEngine->isoMap->tileSize*0.5;
    }
    Texture_RenderXYClipScale(&isoEngine->isoMap->tileSet->tilesTex[0],(isoEngine->zoomLevel*isoEngine->mousePoint.x)-correctX,
                             (isoEngine->zoomLevel*isoEngine->mousePoint.y)+correctY,&isoEngine->isoMap->tileSet->tileClipRects[0],isoEngine->zoomLevel);
}

//Is here for reference to older tutorials. Can be removed if you wish so, since it will be copied and modified to run
//in the render isometric world system in the entity component system.
void IsoEngine_DrawIsoMap(IsoEngine *isoEngine) {
    int i,j;
    int x,y;
    int tile = 4;
    SDL_FPoint point;

    if (isoEngine==NULL) {
        return;
    }
    if (isoEngine->isoMap == NULL) {
        return;
    }

    int startX = -3/isoEngine->zoomLevel +(isoEngine->mapScroll2Dpos.x/isoEngine->zoomLevel/isoEngine->isoMap->tileSize)*2;
    int startY = -20/isoEngine->zoomLevel + fabsf((isoEngine->mapScroll2Dpos.y/isoEngine->zoomLevel/isoEngine->isoMap->tileSize))*2;
    int numTilesInWidth = ((WINDOW_WIDTH/isoEngine->isoMap->tileSize)/isoEngine->zoomLevel);
    int numTilesInHeight = ((WINDOW_HEIGHT/isoEngine->isoMap->tileSize)/isoEngine->zoomLevel)*2;

    if (isoEngine->isoMap->tileSet != NULL) {

        for (i=startY;i < startY+numTilesInHeight+26; ++i) {
            for (j=startX;j < startX+numTilesInWidth+5; ++j) {

                //only draw when both x & y is equal, so we skip here
                if ((j&1) != (i&1)) {
                    continue;
                }
                x = (i+j)/2;
                y = (i-j)/2;

                if (x>=0 && y>=0 && x<isoEngine->isoMap->mapWidth && y<isoEngine->isoMap->mapHeight) {
                    tile = isoMapGetTile(isoEngine->isoMap,x,y,0);
                    point.x = ((x*isoEngine->zoomLevel *isoEngine->isoMap->tileSize) + isoEngine->scrollX);
                    point.y = ((y*isoEngine->zoomLevel *isoEngine->isoMap->tileSize) + isoEngine->scrollY);
                    IsoEngine_Convert2DToIso(&point);
                    Texture_RenderXYClipScale(isoEngine->isoMap->tileSet->tilesTex,point.x,point.y,
                                             &isoEngine->isoMap->tileSet->tileClipRects[tile],isoEngine->zoomLevel);
                }
            }
        }
    }
}

void IsoEngine_GetMouseTilePos(IsoEngine *isoEngine, SDL_FPoint *mouseTilePos) {
    if (isoEngine == NULL) {
        WriteError("isoEngine is NULL!");
        return;
    }
    if (isoEngine->isoMap == NULL) {
        WriteError("isoEngine->isoMap is NULL!");
        return;
    }

    SDL_FPoint point;
    SDL_FPoint tileShift, mouse2IsoPOint;

    if (isoEngine == NULL || mouseTilePos == NULL) {
        return;
    }

    int modulusX = isoEngine->isoMap->tileSize*isoEngine->zoomLevel;
    int modulusY = isoEngine->isoMap->tileSize*isoEngine->zoomLevel;
    int correctX =(((int)isoEngine->mapScroll2Dpos.x)%modulusX)*2;
    int correctY = ((int)isoEngine->mapScroll2Dpos.y)%modulusY;

    //copy mouse point
    mouse2IsoPOint = isoEngine->mousePoint;
    IsoEngine_ConvertIsoTo2D(&mouse2IsoPOint);

    //get tile coordinates
    IsoEngine_GetTileCoordinates(isoEngine,&mouse2IsoPOint,&point);

    tileShift.x = correctX;
    tileShift.y = correctY;
    IsoEngine_Convert2DToIso(&tileShift);

    //check for fixing tile position when the y position is larger than 0
    if (isoEngine->mapScroll2Dpos.y>0) {
        point.y -= (((float)isoEngine->scrollY-tileShift.y)/(float)isoEngine->isoMap->tileSize)/isoEngine->zoomLevel;
        point.y+=1;
    }
    else{
        point.y -= (((float)isoEngine->scrollY-tileShift.y)/(float)isoEngine->isoMap->tileSize)/isoEngine->zoomLevel;
    }

    //check for fixing tile position when the x position is larger than 0
    if (isoEngine->mapScroll2Dpos.x>0) {
        point.x -= (((float)isoEngine->scrollX+(float)tileShift.x)/(float)isoEngine->isoMap->tileSize)/isoEngine->zoomLevel;
        point.x+=1;
    }
    else{
        point.x -= (((float)isoEngine->scrollX+(float)tileShift.x)/(float)isoEngine->isoMap->tileSize)/isoEngine->zoomLevel;
    }
    mouseTilePos->x = (int)point.x;
    mouseTilePos->y = (int)point.y;
}

void IsoEngine_CenterMapToTileUnderMouse(IsoEngine *isoEngine) {
    if (isoEngine == NULL) {
        WriteError("isoEngine is NULL!");
        return;
    }
    if (isoEngine->isoMap == NULL) {
        WriteError("isoEngine->isoMap is NULL!");
        return;
    }
    if (isoEngine->zoomLevel <= 1.0 || isoEngine->zoomLevel>=3.0) {
        return;
    }

    SDL_FPoint mouseIsoTilePos;

    //calculate the offset of the center of the screen
    int offsetX = WINDOW_WIDTH/isoEngine->zoomLevel/2;
    int offsetY = WINDOW_HEIGHT/isoEngine->zoomLevel/2;

    //get the tile under the mouse
    IsoEngine_GetMouseTilePos(isoEngine,&mouseIsoTilePos);

    isoEngine->tilePos.x = mouseIsoTilePos.x*isoEngine->isoMap->tileSize;
    isoEngine->tilePos.y = mouseIsoTilePos.y*isoEngine->isoMap->tileSize;

    //convert to isometric coordinates
    IsoEngine_Convert2DToIso(&mouseIsoTilePos);

    //move the x position
    isoEngine->mapScroll2Dpos.x = ((mouseIsoTilePos.x*isoEngine->isoMap->tileSize)*isoEngine->zoomLevel)/2;
    isoEngine->mapScroll2Dpos.x -= (offsetX*isoEngine->zoomLevel)/2;

    //move the y position
    isoEngine->mapScroll2Dpos.y = -((mouseIsoTilePos.y*isoEngine->isoMap->tileSize)*isoEngine->zoomLevel);
    isoEngine->mapScroll2Dpos.y += offsetY*isoEngine->zoomLevel;

    //convert the map 2d camera to isometric camera
    IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
}

void IsoEngine_CenterMap(IsoEngine *isoEngine,SDL_FPoint *objectPoint,SDL_FPoint *objectSize) {
    SDL_FPoint pointPos = *objectPoint;
    SDL_FPoint objSize;
    int moveAmount = 0;
    //calculate the offset of the center of the screen
    int offsetX = WINDOW_WIDTH/isoEngine->zoomLevel*0.5;
    int offsetY = WINDOW_HEIGHT/isoEngine->zoomLevel*0.5;

    objSize.x = 0;
    objSize.y = 0;

    //if an object size is passed in
    if (objectSize!=NULL) {
        objSize = *objectSize;
    }

    isoEngine->tilePos.x = objectPoint->x;
    isoEngine->tilePos.y = objectPoint->y;

    IsoEngine_Convert2DToIso(&pointPos);

    isoEngine->mapScroll2Dpos.x = SDL_floor((pointPos.x)*isoEngine->zoomLevel)*0.5;
    isoEngine->mapScroll2Dpos.x -= offsetX*isoEngine->zoomLevel*0.5;

    if (isoEngine->gameMode == GAME_MODE_OBJECT_FOCUS) {
        moveAmount = (int)(objSize.x*isoEngine->zoomLevel*0.5);
        isoEngine->mapScroll2Dpos.x += moveAmount*0.5;
    }

    isoEngine->mapScroll2Dpos.y = -SDL_floor((pointPos.y)*isoEngine->zoomLevel);
    isoEngine->mapScroll2Dpos.y += offsetY*isoEngine->zoomLevel;

    if (isoEngine->gameMode == GAME_MODE_OBJECT_FOCUS) {

        moveAmount = (int)(objSize.y*isoEngine->zoomLevel*0.5);
        isoEngine->mapScroll2Dpos.y -= moveAmount;
    }

    IsoEngine_ConvertCartesianCameraToIsometric(isoEngine,&isoEngine->mapScroll2Dpos);
}


void IsoEngine_GetMouseTileClick(IsoEngine *isoEngine) {
    if (isoEngine == NULL) {
        WriteError("isoEngine is NULL!");
        return;
    }
    if (isoEngine->isoMap == NULL) {
        WriteError("isoEngine->isoMap is NULL!");
        return;
    }
    SDL_FPoint point;
    //initialize the point values (removes memory leak warnings with valgrind)
    point.x=0;
    point.y=0;
    IsoEngine_GetMouseTilePos(isoEngine,&point);
    if (point.x>=0 && point.y>=0 && point.x<isoEngine->isoMap->mapWidth && point.y<isoEngine->isoMap->mapHeight) {
        isoEngine->lastTileClicked = isoMapGetTile(isoEngine->isoMap,(int)point.x,(int)point.y,0);
    }
}

void IsoEngine_ZoomIn(IsoEngine *isoEngine) {
    if (isoEngine->zoomLevel<3.0) {
        isoEngine->zoomLevel+=0.25;

        if (isoEngine->gameMode==GAME_MODE_OVERVIEW) {
            IsoEngine_CenterMap(isoEngine,&isoEngine->tilePos,NULL);
        }
    }
}
void IsoEngine_ZoomOut(IsoEngine *isoEngine) {
    if (isoEngine->zoomLevel>1.0) {
        isoEngine->zoomLevel-=0.25;

        if (isoEngine->gameMode==GAME_MODE_OVERVIEW) {
            IsoEngine_CenterMap(isoEngine,&isoEngine->tilePos,NULL);
        }
    }
}

void IsoEngine_SetGameMode(IsoEngine *isoEngine,IsoEngineGameMode gameMode) {
    if (isoEngine == NULL) {
        return;
    }
    isoEngine->gameMode = gameMode;
}
