#ifndef __TIMER_H
#define __TIMER_H

typedef struct Timer {
    float currentTime;     //current time
    float timeLog;         //point in time to count from
    float timeDuration;    //how long shall the timer count
} Timer;

//this function initializes the timer and set how long it shall wait in milliseconds
void Timer_Init(Timer *timer, int ms);

//this function updates the timer and resets it when it comes to its end.
int Timer_Update(Timer *timer);

#endif // __TIMER_H
