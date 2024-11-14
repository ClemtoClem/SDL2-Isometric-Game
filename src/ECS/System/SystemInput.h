#ifndef __INPUT_SYSTEM_H_
#define __INPUT_SYSTEM_H_
#include <SDL2/SDL.h>

int SystemInput_Init(void *scene);
void SystemInput_Update();
void SystemInput_UpdateEntity(Uint32 entity);
void SystemInput_Free();
#endif // __INPUT_SYSTEM_H_

