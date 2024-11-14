#include <SDL2/SDL.h>
#include "DeltaTimer.h"

static DeltaTimer deltaTimer;

void DeltaTimer_Init() {
    //initialize the values to 0.0
    deltaTimer.deltaTime = 0.0;
    deltaTimer.oldTick = 0.0;
    deltaTimer.tick = 0.0;
}

void DeltaTimer_Update() {
    //get the old tick time
    deltaTimer.oldTick = deltaTimer.tick;

    //get current tick time
    deltaTimer.tick = SDL_GetTicks();

    //get time duration between ticks and divide by 1000
    //to get number of seconds since last frame
    deltaTimer.deltaTime = (deltaTimer.tick - deltaTimer.oldTick) / 1000.0;
}

double DeltaTimer_GetDeltaTime() {
    //return the delta time
    return deltaTimer.deltaTime;
}
