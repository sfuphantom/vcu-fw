/*
 * phantom_timer.h
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#ifndef PHANTOM_TIMER_H_
#define PHANTOM_TIMER_H_

#include "vcu_common.h"

#include "hal_stdtypes.h"

#include "os_timer.h"

#define AUTO_RELOAD true
#define NO_RELOAD   false 

typedef uint16_t TimerID_t;

/*
    timerName:          Just a text name, not used by the RTOS kernel.
    periodMS:           The timer period in ticks, must be greater than 0.
    isAutoReloading:    If true, the timer will auto-reload itself when they expire.
    counterPtr:         Pointer to a number which increments whenever the timer expires, initialized to 0.
    callbackFunction:   Callback function that is called when the timer expires.
*/
TimerHandle_t Phantom_createTimer(char* const timerName, 
                                  uint32 periodMS, 
                                  bool isAutoReloading, 
                                  TimerID_t timerID,
                                  TimerCallbackFunction_t callbackFunction);

uint8 Phantom_startTimer(TimerHandle_t timer, uint32 waitTimeMs);
uint8 Phantom_stopTimer(TimerHandle_t timer, uint32 waitTimeMs);
uint8 Phantom_deleteTimer(TimerHandle_t timer, uint32 waitTimeMs);

int Phantom_getNumberOfTimers(void);
bool Phantom_isTimerActive(TimerHandle_t timer);

#endif
