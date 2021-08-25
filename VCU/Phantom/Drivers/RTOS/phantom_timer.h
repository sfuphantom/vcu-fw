#ifndef PHANTOM_TIMER_H_
#define PHANTOM_TIMER_H_

#include "os_timer.h"

/*
    timerName:          Just a text name, not used by the RTOS kernel.
    periodMS:           The timer period in ticks, must be greater than 0.
    isAutoReloading:    If true, the timer will auto-reload itself when they expire.
    counterPtr:         Pointer to a number which increments whenever the timer expires, initialized to 0.
    callbackFunction:   Callback function that is called when the timer expires.
*/
int Phantom_createTimer(char* timerName, 
                        unsigned int periodMS, 
                        int isAutoReloading, 
                        void* counterPtr, 
                        TimerCallbackFunction_t callbackFunction);

#endif