#ifndef __RENDERER_H
#define __RENDERER_H

#define WINDOW_WIDTH     1280
#define WINDOW_HEIGHT    720

void initRenderer(char *windowCaption);
[[nodiscard]] SDL_Renderer *getRenderer();
[[nodiscard]] SDL_Window *getWindow();
void closeRenderer();


#endif // __RENDERER_H
