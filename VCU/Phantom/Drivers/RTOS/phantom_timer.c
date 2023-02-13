/*
 * phantom_timer.c
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */


#include "phantom_timer.h"
#include "task_logger.h"
#include "state_machine.h"

// #define DEBUG    // uncomment this line for ASSERT statements to work

#define MAX_NUMBER_OF_TIMERS    10

static int numOfTimers = 0;

/*
    timerName:          Just a text name, not used by the RTOS kernel.
    periodMS:           The timer period in ticks, must be greater than 0.
    isAutoReloading:    If true, the timer will auto-reload itself when they expire.
    timerID:         Pointer to a number which increments whenever the timer expires, initialized to 0.
    callbackFunction:   Callback function that is called when the timer expires.
*/
TimerHandle_t Phantom_createTimer(char* const timerName, 
                                  uint32 periodMS, 
                                  bool isAutoReloading, 
                                  TimerID_t timerID,
                                  TimerCallbackFunction_t callbackFunction)
{
    if (numOfTimers >= MAX_NUMBER_OF_TIMERS) {
        return NULL;
    }

    TimerHandle_t newTimer = xTimerCreate( 
        timerName,
        pdMS_TO_TICKS(periodMS),
        isAutoReloading,
        (void*)timerID,
        callbackFunction
    );

    numOfTimers += (newTimer != NULL);
    return newTimer;
}

static uint8 LogTimerInfo(TimerHandle_t timer, char* str)
{
    char buffer[32];
    snprintf(buffer, 32, "(%s) %s", pcTimerGetName(timer), str);

    return Log(buffer);
}

uint8 Phantom_stopTimer(TimerHandle_t timer, uint32 waitTimeMs)
{
    uint8_t ret = xTimerStop(timer, pdMS_TO_TICKS(waitTimeMs)) == pdPASS;

    if (!ret)
    {
        LogTimerInfo(timer, "Failed to stop timer");
    }
    
    return ret;
}

uint8 Phantom_deleteTimer(TimerHandle_t timer, uint32 waitTimeMs)
{
    uint8 result = xTimerDelete(timer, pdMS_TO_TICKS(waitTimeMs));
    numOfTimers -= (result == pdPASS);
    return result;
}

int Phantom_getNumberOfTimers(void)
{
    return numOfTimers;
}

bool Phantom_isTimerActive(TimerHandle_t timer)
{
    uint8_t ret = xTimerIsTimerActive(timer) == pdPASS;
    
    return ret;
}

uint8 Phantom_startTimer(TimerHandle_t timer, uint32 waitTimeMs)
{
    uint8_t started = true;

    if (!Phantom_isTimerActive(timer))
    {
        started = xTimerStart(timer, pdMS_TO_TICKS(waitTimeMs)) != pdFALSE;
    }


    if (!started)
    {
        LogTimerInfo(timer, "Timer not started");
    }

    return started;
    
}
