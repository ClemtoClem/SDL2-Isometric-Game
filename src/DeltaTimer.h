#ifndef __DELTA_TIMER_H
#define __DELTA_TIMER_H

#include <SDL2/SDL.h>

typedef struct DeltaTimer {
    Uint32 oldTick;
    Uint32 tick;
    double deltaTime;
} DeltaTimer;

void DeltaTimer_Init();
void DeltaTimer_Update();
[[nodiscard]] double DeltaTimer_GetDeltaTime();

#endif // __DELTA_TIMER_H