#ifndef TEXTURE_POOL_H
#define TEXTURE_POOL_H

#include <SDL2/SDL.h>
#include "Texture.h"

#define NUM_INITIAL_TEXTURES_IN_TEXTUREPOOL 1

typedef struct TextureContainer {
    Texture *texture;      //pointer to the texture data
    char *name;             //name of the texture
} TextureContainer;

typedef struct TexturePool {
    int numTextures;                //number of textures
    int maxTextures;                //current max number of textures
    TextureContainer *textures;    //pointer to the textures
} TexturePool;

TexturePool *TexturePool_New();
void TexturePool_AddTexture(TexturePool *texturePool,char *filename);
void TexturePool_Remove(TexturePool *texturePool,char *filename);
Texture *TexturePool_GetTexture(TexturePool *texturePool,char *filename);
void TexturePool_Free(TexturePool *texturePool);

#endif // TEXTURE_POOL_H
