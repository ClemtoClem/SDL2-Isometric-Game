#include <stdlib.h>
#include <stdio.h>
#include "FontPool.h"
#include "logger.h"

static FontPool *fontPool = NULL;

void FontPool_SetPointer(FontPool *newFontPool) {
    //set the fontPool pointer
    fontPool = newFontPool;
}

FontPool *FontPool_GetPointer() {
    //return the font pool
    return fontPool;
}

FontPool *FontPool_NewFontPool(int initialNumFonts) {
    int i;
    FontPool *fontPool;

    //allocate memory for the font pool
    fontPool = malloc(sizeof(struct FontPool));
    //if memory allocation failed
    if (fontPool == NULL) {
        WriteError( "FontPool_NewFontPool", "Could not allocate memory for the font pool.");
        return NULL;
    }

    //allocate memory for the fonts
    fontPool->fonts = malloc(sizeof(struct Font)*initialNumFonts);
    //if memory allocation failed
    if (fontPool->fonts == NULL) {
        WriteError("Could not allocate memory for the requested %d fonts in the font pool.",initialNumFonts);
        return NULL;
    }
    //initialize the fonts
    for (i = 0; i < initialNumFonts; ++i) {
        fontPool->fonts[i].texture.texture = NULL;
        fontPool->fonts[i].cHeight = 0;
        fontPool->fonts[i].cWidth = 0;
    }
    fontPool->numFonts = 0;
    fontPool->maxFonts = initialNumFonts;
    return fontPool;
}

int FontPool_AddFont(FontPool *fontPool,char *textureFilename,char *fontName,int cWidth,int cHeight,int fontSpace) {
    int i = 0;    int x=0,y=0;
    Font *newFonts = NULL;

    //make sure the font pool exist
    if (fontPool==NULL) {
        return 0;
    }

    //allocate more memory if necessary
    if (fontPool->numFonts >= fontPool->maxFonts) {
        //try to allocate memory for one more font
        fontPool->maxFonts++;
        newFonts = realloc(fontPool->fonts,sizeof(struct Font)*fontPool->maxFonts);

        //if memory allocation failed
        if (newFonts == NULL) {
            WriteError( "FontPool_AddFont", "Could not reallocate more memory for font pool!");
            return 0;
        }
        //point the fonts to the new memory location
        fontPool->fonts = newFonts;
    }

    //load and initialize the texture
    Texture_loadFromFile(&fontPool->fonts[fontPool->numFonts].texture,textureFilename);
    Texture_Init(&fontPool->fonts[fontPool->numFonts].texture,0,0,0,NULL,NULL,SDL_FLIP_NONE);

    //get the character height
    fontPool->fonts[fontPool->numFonts].cWidth = cWidth;
    fontPool->fonts[fontPool->numFonts].cHeight = cHeight;

    //write in the texture filename
    sprintf(fontPool->fonts[fontPool->numFonts].name,"%s",fontName);

    //set font space for all characters
    for (i = 0; i < NUM_FONT_CHARS; ++i) {
        fontPool->fonts[fontPool->numFonts].space[i] = fontSpace;
    }

    //set up the clip rectangles for all the characters
    for (i = 0; i < NUM_FONT_CHARS; ++i) {
        SetupRect(&fontPool->fonts[fontPool->numFonts].c[i],x,y,cWidth,cHeight);
        x+=cWidth;
        if (x >= fontPool->fonts[fontPool->numFonts].texture.width) {
            x=0;
            y+=cHeight;
        }
    }
    //increase number of fonts in the pool
    fontPool->numFonts++;
    return 1;
}


//this function frees the font pool and the fonts in it.
void FontPool_Free(FontPool *fontPool) {
    int i = 0, j = 0;
    if (fontPool != NULL) {
        //free the fonts
        if (fontPool->fonts != NULL) {
            free(fontPool->fonts);
        }

        //free the font pool
        free(fontPool);
    }
}


Font *FontPool_GetFontByName(FontPool *fontPool,char *fontName) {
    int i = 0;

    //If the font name is null
    if (fontName == NULL) {
        //return null
        return NULL;
    }

    //loop through the font pool
    for (i = 0; i < fontPool->numFonts; ++i) {
        //if the font is found
        if (strcmp(fontName,fontPool->fonts[i].name)==0) {
            //return it
            return &fontPool->fonts[i];
        }
    }
    //if the font was not found
    WriteError("Could not find the font:%s in the font pool!",fontName);
    //return null;
    return NULL;
}

void FontPool_SetCharacterFontSpace(FontPool *fontPool,char *fontName,char character,int space) {
    Font *font;
    //map the value to the font layout in the bitmap
    int charvalue = (int)character - 32;

    //if the font pool is not allocated
    if (fontPool == NULL) {
        WriteError( "FontPool_SetCharacterFontSpace", "Parameter 'fontPool *fontPool' is NULL");
        return;
    }
    //if the font name is not valid
    if (fontName == NULL) {
        WriteError( "FontPool_SetCharacterFontSpace", "Parameter 'char *fontName' is NULL");
        return;
    }

    //get the font by name
    font = FontPool_GetFontByName(fontPool,fontName);
    //if the font was not found
    if (font == NULL) {
        WriteError("Could not find font:%s in the font pool!",fontName);
        return;
    }

    //set the character space
    font->space[charvalue] = space;
}

void BitmapFontString(Font *font,char *string,int x,int y) {
    int i = 0;    int length=0;
    int c=0;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError("BitmapFontString", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError("BitmapFontString", "Parameter 'char *string' is NULL");
        return;
    }

    //get the length of the string
    length = strlen(string);
    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClip(&font->texture,x,y,&font->c[c]);
            //move to the right with the earlier given space between each character
            x+=font->space[c];
        }
        else
        {
            //draw the character
            Texture_RenderXYClip(&font->texture,x,y,&font->c[0]);

            //move to the right with the earlier given space between each character
            x+=font->space[0];
        }
    }
}

void BitmapFontStringColor(Font *font,char *string,int x,int y,SDL_Color color) {
    int i = 0;    int length=0;
    int c=0;

    SDL_Color original;
    original.a = 0xff;
    original.r = 0xff;
    original.g = 0xff;
    original.b = 0xff;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError( "BitmapFontString", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError( "BitmapFontString", "Parameter 'char *string' is NULL");
        return;
    }

    //get the length of the string
    length = strlen(string);

    //get the current color mod
    SDL_GetTextureColorMod(font->texture.texture,&original.r,&original.g,&original.b);

    //set the new color
    SDL_SetTextureColorMod(font->texture.texture,color.r,color.g,color.b);

    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClip(&font->texture,x,y,&font->c[c]);
            //move to the right with the earlier given space between each character
            x+=font->space[c];
        }
        else
        {
            //draw the character
            Texture_RenderXYClip(&font->texture,x,y,&font->c[0]);

            //move to the right with the earlier given space between each character
            x+=font->space[0];
        }
    }
    //restore the original color
    SDL_SetTextureColorMod(font->texture.texture,original.r,original.g,original.b);
}

void BitmapFontStringScale(Font *font,char *string,int x,int y,float scale) {
    int i = 0;    int length=0;
    int c=0;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError( "BitmapFontString", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError( "BitmapFontString", "Parameter 'char *string' is NULL");
        return;
    }
    //if scale is less or equal to 0
    if (scale <= 0) {
        //set the scale to 1.0
        scale = 1.0;
    }

    //get the length of the string
    length = strlen(string);
    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[c],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[c]*scale;
        }
        else
        {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[0],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[0]*scale;
        }
    }
}
void BitmapFontStringScaleColor(Font *font,char *string,int x,int y,float scale,SDL_Color color) {
    int i = 0;    int length=0;
    int c=0;

    SDL_Color original;
    original.a = 0xff;
    original.r = 0xff;
    original.g = 0xff;
    original.b = 0xff;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError( "BitmapFontString", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError( "BitmapFontString", "Parameter 'char *string' is NULL");
        return;
    }
    //if scale is less or equal to 0
    if (scale <= 0) {
        //set the scale to 1.0
        scale = 1.0;
    }

    //get the current color mod
    SDL_GetTextureColorMod(font->texture.texture,&original.r,&original.g,&original.b);

    //set the new color
    SDL_SetTextureColorMod(font->texture.texture,color.r,color.g,color.b);

    //get the length of the string
    length = strlen(string);

    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[c],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[c]*scale;
        }
        else
        {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[0],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[0]*scale;
        }
    }

    //restore the original color
    SDL_SetTextureColorMod(font->texture.texture,original.r,original.g,original.b);
}
void BitmapFontStringCenterScale(Font *font,char *string,int x,int y,float scale) {
    int i = 0;    int length=0;
    int c=0;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError( "BitmapFontStringCenterScale", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError( "BitmapFontStringCenterScale", "Parameter 'char *string' is NULL");
        return;
    }
    //if scale is less or equal to 0
    if (scale <= 0) {
        //set the scale to 1.0
        scale = 1.0;
    }

    //get the length of the string
    length = strlen(string);

    //calculate the center

    x=x - font->space[0] * length/2;

    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[c],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[c]*scale;
        }
        else
        {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[0],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[0]*scale;
        }
    }
}
void BitmapFontStringCenterScaleColor(Font *font,char *string,int x,int y,float scale,SDL_Color color) {
    int i = 0;    int length=0;
    int c=0;

    SDL_Color original;
    original.a = 0xff;
    original.r = 0xff;
    original.g = 0xff;
    original.b = 0xff;

    //exit the function if the font or string is NULL
    if (font == NULL) {
        WriteError( "BitmapFontStringCenterScaleColor", "Parameter 'Font *font' is NULL");
        return;
    }
    if (string == NULL) {
        WriteError( "BitmapFontStringCenterScaleColor", "Parameter 'char *string' is NULL");
        return;
    }
    //if scale is less or equal to 0
    if (scale <= 0) {
        //set the scale to 1.0
        scale = 1.0;
    }

    //get the length of the string
    length = strlen(string);

    //calculate the center
    x=x - font->space[0] * length/2;

    //get the current color mod
    SDL_GetTextureColorMod(font->texture.texture,&original.r,&original.g,&original.b);

    //set the new color
    SDL_SetTextureColorMod(font->texture.texture,color.r,color.g,color.b);

    for (i = 0; i < length; ++i) {
        //map the character to the character in the texture
        //in the texture the 32 first letters are skipped in the ASCII table,
        //so a rollback of 32 characters is needed
        c = string[i] - 32;

        if (c >= 0 && c  <=  NUM_FONT_CHARS) {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[c],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[c]*scale;
        }
        else
        {
            //draw the character
            Texture_RenderXYClipScale(&font->texture,x,y,&font->c[0],scale);
            //move to the right with the earlier given space between each character
            x+=font->space[0]*scale;
        }
    }
    //restore the original color
    SDL_SetTextureColorMod(font->texture.texture,original.r,original.g,original.b);
}

SDL_Color FontPool_GetColor(int r,int g,int b) {
    SDL_Color c;
    //keep the values within range
    if (r > 255) { r = 255; }
    if (g > 255) { g = 255; }
    if (b > 255) { b = 255; }

    if (r < 0) { r = 0; }
    if (g < 0) { g = 0; }
    if (b < 0) { b = 0; }

    //set the values
    c.a = 0xff;
    c.r = r;
    c.g = g;
    c.b = b;

    return c;
}
