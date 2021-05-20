/*
 * throttle.h
 *
 *  Created on: May 5, 2021
 *      Author: jaypacamarra
 */

#ifndef THROTTLE_H_
#define THROTTLE_H_

#include "vcu_data.h"
#include "board_hardware.h"
#include "phantom_freertos.h"
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include <adc.h>


/* For calculating throttle padding */
#define PADDING_PERCENT     0.08 // Must be between 0.0 and 1.0

/* Brake Light readability */
#define BRAKE_LIGHT_ON      0
#define BRAKE_LIGHT_OFF     1


void getPedalReadings();
void calculatePedalPercents();
void applyLowPassFilter();
bool check_BSE_Range_Fault();
bool check_APPS1_Range_Fault();
bool check_APPS2_Range_Fault();
bool check_10PercentAPPS_Fault();
bool check_Brake_Plausibility_Fault();


#endif /* THROTTLE_H_ */
