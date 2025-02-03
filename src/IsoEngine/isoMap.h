#ifndef __ISO_MAP_H
#define __ISO_MAP_H

#include <SDL2/SDL.h>
#include "../Texture.h"

#define MAP_NAME_LENGTH 50
#define NUM_TILES_PER_ROW_IN_TILESET    23
#define NUM_TILE_LEVELS_PER_LAYER       6

typedef struct IsoTileSet {
    int tileSetLoaded;
    int numTileClipRects;
    Texture *tilesTex;
    SDL_Rect *tileClipRects;
} IsoTileSet;

typedef struct IsoMap {
    int mapHeight;
    int mapWidth;
    int numLayers;
    int tileSize;
    int tileSizeX;
    int tileSizeY;
    int *mapData;
    char name[MAP_NAME_LENGTH];
    IsoTileSet *tileSet;
} IsoMap;

[[nodiscard]] IsoMap* isoMapCreateNewMap(char *mapName,int width,int height,int numLayers,int tileSize,int perlinSeed, int terrainHeight);
void isoMapFreeMap(IsoMap *isoMap);
int isoMapLoadTileSet(IsoMap *isoMap,Texture *texture,int tileWidth,int tileHeight);
[[nodiscard]] int isoMapGetTile(IsoMap *isoMap,int x,int y,int layer);
void isoMapSetTile(IsoMap *isoMap,int x,int y,int layer,int value);

#endif // __ISO_MAP_H

