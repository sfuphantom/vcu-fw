/*
 * phantom_timer.c
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#include "sys_common.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "os_timer.h"

#include "phantom_timer.h"

// #define DEBUG    // uncomment this line for ASSERT statements to work

#define MAX_NUMBER_OF_TIMERS   2
static TimerHandle_t xTimers[MAX_NUMBER_OF_TIMERS];
static int xTimersSize = 0;


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
                                  TimerCallbackFunction_t callbackFunction)
{
    if (xTimersSize >= MAX_NUMBER_OF_TIMERS) {
        // TODO: Print to console an error message
        return NULL;
    }

    TimerHandle_t newTimer = xTimerCreate( 
        timerName,
        pdMS_TO_TICKS(periodMS),
        isAutoReloading,
        counterPtr,
        callbackFunction
    );

    if (newTimer == NULL) {
        // TODO: Print to console an error message
        return NULL;
    }

    if (xTimerStart(newTimer, 0) != pdPASS) {
        // TODO: Print to console an error message
        return NULL;
    }

    xTimers[xTimersSize] = newTimer;
    xTimersSize++;

    return newTimer;
}