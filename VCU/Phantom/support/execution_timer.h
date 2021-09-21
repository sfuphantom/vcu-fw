/*
 * execution_timer.h
 *
 *  Created on: Mar 18, 2021
 *      Author: junaidkhan
 */

#ifndef PHANTOM_SUPPORT_EXECUTION_TIMER_H_
#define PHANTOM_SUPPORT_EXECUTION_TIMER_H_

#include "sys_pmu.h"
#include "../Drivers/EEPROM/eeprom_driver.h"


//#define PMU_CYCLE


extern void swiSwitchToMode(uint32 mode);

/*
 *  Note: PMU has 3 event counters and 1 counter dedicated for only CPU cycles.
 *        This code assumes you only want the CPU clock cycles for a section of code enclosed between timer_Start() and timer_Stop().
 *        timer_Start() resets ALL the counters, so only use timer_Start() for one thread ONLY -> still working on more dynamic behavior.
 *
 */

inline void timer_Init(void){
    /* Initialize Code Execution time Measuring Module */
    _pmuInit_();
    _pmuEnableCountersGlobal_();
}

inline unsigned long timer_Start(void){

    swiSwitchToMode(SYSTEM_MODE);
    /* Start Execution Time Counter. */
    unsigned long cycles_start;
    _pmuResetCounters_();
    _pmuStartCounters_(pmuCYCLE_COUNTER);
    cycles_start = _pmuGetCycleCount_();
    swiSwitchToMode(USER_MODE);
    return cycles_start;
}

inline float timer_Stop(unsigned long cycles_START, float CPU_CLOCK_MHz)
{
    /* ++ stop Execution Time Counter. */

    unsigned long  cycles_END, cycles_PMU_measure, cycles_PMU_end, cycles_PMU_start, cycles_PMU_compensation, corrected_cycles;
    float time_uSECOND;

    swiSwitchToMode(SYSTEM_MODE);

    _pmuStopCounters_(pmuCYCLE_COUNTER);
    cycles_END = _pmuGetCycleCount_();
    cycles_PMU_measure = cycles_END - cycles_START; // uncompensated clock cycles between start and stop timer
    /* -- stop Execution Time Counter. */


    /* ++ Compensation time for starting and stopping timer. */
    _pmuResetCounters_();
    _pmuGetCycleCount_();
    cycles_PMU_start = _pmuGetCycleCount_();

    _pmuStopCounters_(pmuCYCLE_COUNTER);
    cycles_PMU_end = _pmuGetCycleCount_();
    cycles_PMU_compensation = cycles_PMU_end - cycles_PMU_start;
    /* -- Compensation time for starting and stopping timer     */


    /* ++ Calculate execution time. */
    corrected_cycles = cycles_PMU_measure - cycles_PMU_compensation;
    time_uSECOND = corrected_cycles/(CPU_CLOCK_MHz); // execution time in [us], CPU_CLOCK_MHZ = [MHz]
    /* -- Calculate execution time. */
    swiSwitchToMode(USER_MODE);

    return time_uSECOND;
}


#endif /* PHANTOM_SUPPORT_EXECUTION_TIMER_H_ */
