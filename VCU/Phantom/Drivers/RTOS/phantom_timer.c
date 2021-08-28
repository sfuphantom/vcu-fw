/*
 * phantom_timer.c
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_timer.h"

#include "phantom_timer.h"

// #define DEBUG    // uncomment this line for ASSERT statements to work

#define WAIT_TIME_MS    1
#define MAX_NUMBER_OF_TIMERS    10

static int numOfTimers = 0;

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
    if (numOfTimers >= MAX_NUMBER_OF_TIMERS) {
        return NULL;
    }

    TimerHandle_t newTimer = xTimerCreate( 
        timerName,
        pdMS_TO_TICKS(periodMS),
        isAutoReloading,
        counterPtr,
        callbackFunction
    );

    numOfTimers += (newTimer != NULL);
    return newTimer;
}

uint8 Phantom_startTimer(TimerHandle_t timer)
{
    return xTimerStart(timer, pdMS_TO_TICKS(WAIT_TIME_MS));
}

uint8 Phantom_stopTimer(TimerHandle_t timer)
{
    return xTimerStop(timer, pdMS_TO_TICKS(WAIT_TIME_MS));
}

uint8 Phantom_deleteTimer(TimerHandle_t timer)
{
    uint8 result = xTimerDelete(timer, pdMS_TO_TICKS(WAIT_TIME_MS));
    numOfTimers -= (result == pdPASS);
    return result;
}

int Phantom_getNumberOfTimers(void)
{
    return numOfTimers;
}

uint8 Phantom_isTimerActive(TimerHandle_t timer)
{
    return xTimerIsTimerActive(timer);
}