#ifndef __FONTPOOL_H
#define __FONTPOOL_H

#include <SDL2/SDL.h>
#include "Texture.h"

#define NUM_FONT_CHARS 96

/* FONT TEMPLATE FOR THE STORED CHARACTERS WITH THEIR NUMBERS IN THE ARRAY

        //    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,

        // 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
          16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,

        // @  A	 B  C  D  E  F  G  H  I  J  K  L  M  N  O
          32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,

        // P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
          48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,

        // `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
          64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,

        // p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~
          80,81,82,83,84,86,86,87,88,89,90,91,92,93,94,95,
*/

//structure of a font
typedef struct Font {
    int cWidth;                     //the width of each character in the texture
    int cHeight;                    //the height of each character in the texture
    int space[NUM_FONT_CHARS];      //how many pixels to move forward after each character
    Texture texture;               //the texture holding the characters
    SDL_Rect c[NUM_FONT_CHARS];     //clip rectangle for every character
    char name[255];                 //name of the font
}Font;

//structure of the font pool
typedef struct FontPool {
    Font *fonts;       //pointer to the fonts
    int numFonts;       //current number of fonts in the font pool
    int maxFonts;       //current max number of fonts in the font pool
} FontPool;

[[nodiscard]] FontPool *FontPool_NewFontPool(int initialNumFonts);

int FontPool_AddFont(FontPool *fontPool,char *textureFilename,char *fontName,int cWidth,int cHeight,int fontSpace);
void FontPool_SetPointer(FontPool *newFontPool);
[[nodiscard]] FontPool *FontPool_GetPointer();
void FontPool_Free(FontPool *fontPool);
[[nodiscard]] Font *FontPool_GetFontByName(FontPool *fontPool,char *fontName);
void FontPool_SetCharacterFontSpace(FontPool *fontPool,char *fontName,char character,int space);
void FontPool_SetPointer(FontPool *newFontPool);

void BitmapFontString(Font *font,char *string,int x,int y);
void BitmapFontStringColor(Font *font,char *string,int x,int y,SDL_Color color);
void BitmapFontStringScale(Font *font,char *string,int x,int y,float scale);
void BitmapFontStringScaleColor(Font *font,char *string,int x,int y,float scale,SDL_Color color);
void BitmapFontStringCenterScale(Font *font,char *string,int x,int y,float scale);
void BitmapFontStringCenterScaleColor(Font *font,char *string,int x,int y,float scale,SDL_Color color);

[[nodiscard]] SDL_Color FontPool_GetColor(int r,int g,int b);


#endif // __FONTPOOL_H
