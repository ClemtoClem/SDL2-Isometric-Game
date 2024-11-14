#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <SDL2/SDL.h>

typedef struct Texture {
    int x;
    int y;
    int width;
    int height;
    double angle;
    SDL_Point center;
    SDL_Rect cliprect;
    SDL_RendererFlip fliptype;
    SDL_Texture *texture;
} Texture;

int Texture_loadFromFile(Texture *texture, char *filename);
int Texture_Init(Texture *texture, int x,int y, double angle, SDL_Point *center, SDL_Rect *cliprect, SDL_RendererFlip fliptype);
void Texture_RenderXYClip(Texture *texture, int x, int y, SDL_Rect *cliprect);
void Texture_RenderXYClipScale(Texture *texture, int x, int y, SDL_Rect *cliprect,float scale);
void Texture_Delete(Texture *texture);
void SetupRect(SDL_Rect *rect,int x,int y,int w,int h);

#endif // __TEXTURE_H
