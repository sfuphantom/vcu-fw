/*
 * phantom_timer.h
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#ifndef PHANTOM_TIMER_H_
#define PHANTOM_TIMER_H_

#include "hal_stdtypes.h"
#include "os_timer.h"

/*
    timerName:          Just a text name, not used by the RTOS kernel.
    periodMS:           The timer period in ticks, must be greater than 0.
    isAutoReloading:    If true, the timer will auto-reload itself when they expire.
    counterPtr:         Pointer to a number which increments whenever the timer expires, initialized to 0.
    callbackFunction:   Callback function that is called when the timer expires.
*/
TimerHandle_t Phantom_createTimer(char* const timerName, 
                                  unsigned int periodMS, 
                                  int isAutoReloading, 
                                  void* counterPtr, 
                                  TimerCallbackFunction_t callbackFunction);

#endif