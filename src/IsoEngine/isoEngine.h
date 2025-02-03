#ifndef __ISOENGINE_H
#define __ISOENGINE_H

#include <SDL2/SDL.h>
#include "isoMap.h"

typedef enum IsoEngineGameMode {
    GAME_MODE_OVERVIEW = 0,
    GAME_MODE_OBJECT_FOCUS,
    NUM_GAME_MODES,
} IsoEngineGameMode;

typedef struct IsoEngine {
    int scrollX;
    int scrollY;
    int mapScrollSpeed;
    SDL_FPoint mapScroll2Dpos;
    float zoomLevel;
    SDL_Rect mouseRect;
    SDL_FPoint mousePoint;
    SDL_FPoint tilePos;
    int lastTileClicked;
    IsoMap*isoMap;
    int gameMode;
} IsoEngine;

[[nodiscard]] IsoEngine *IsoEngine_New();
void IsoEngine_Init(IsoEngine *isoEngine, int tileSizeInPixels);
void IsoEngine_Free(IsoEngine *isoEngine);

void IsoEngine_SetMapSize(IsoEngine *isoEngine,int width, int height);
void IsoEngine_Convert2DToIso(SDL_FPoint *point);
void IsoEngine_ConvertIsoTo2D(SDL_FPoint *point);
void IsoEngine_GetTileCoordinates(IsoEngine *isoEngine,SDL_FPoint *point,SDL_FPoint *point2DCoord);

void IsoEngine_ConvertIsoCameraToCartesian(IsoEngine *isoEngine,SDL_FPoint *cartesianCamPos);
void IsoEngine_ConvertCartesianCameraToIsometric(IsoEngine *isoEngine,SDL_FPoint *cartesianCamPos);

void IsoEngine_ConvertIsoPoint2DToCartesian(IsoEngine *isoEngine,SDL_FPoint *isoPoint,SDL_FPoint *cartesianPoint);

void IsoEngine_UpdateMousePos(IsoEngine *isoEngine);
void IsoEngine_ScrollMapWithMouse(IsoEngine *isoEngine);
void IsoEngine_DrawIsoMouse(IsoEngine *isoEngine);
void IsoEngine_DrawIsoMap(IsoEngine *isoEngine);
void IsoEngine_GetMouseTilePos(IsoEngine *isoEngine, SDL_FPoint *mouseTilePos);
void IsoEngine_CenterMapToTileUnderMouse(IsoEngine *isoEngine);
void IsoEngine_CenterMap(IsoEngine *isoEngine,SDL_FPoint *objectPoint,SDL_FPoint *objSize);
void IsoEngine_GetMouseTileClick(IsoEngine *isoEngine);
void IsoEngine_ZoomIn(IsoEngine *isoEngine);
void IsoEngine_ZoomOut(IsoEngine *isoEngine);
void IsoEngine_SetGameMode(IsoEngine *isoEngine, IsoEngineGameMode gameMode);

#endif // __ISOENGINE_H
