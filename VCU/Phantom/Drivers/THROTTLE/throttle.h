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
/* Padding will eliminate unintended range faults at 0% or 100% pedal presses */
#define PADDING_PERCENT         (0.08f) // Must be between 0.0 and 1.0
#define PADDED_BSE_MIN_VALUE    (BSE_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_BSE_MAX_VALUE    (BSE_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS1_MIN_VALUE  (APPS1_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS1_MAX_VALUE  (APPS1_MAX_VALUE * (1U - PADDING_PERCENT))
#define PADDED_APPS2_MIN_VALUE  (APPS2_MIN_VALUE * (1U + PADDING_PERCENT))
#define PADDED_APPS2_MAX_VALUE  (APPS2_MAX_VALUE * (1U - PADDING_PERCENT))

/* Hysteresis */
#define HYSTERESIS      (200U)

/* Brake Light readability */
#define BRAKE_LIGHT_ON      0
#define BRAKE_LIGHT_OFF     1

/* Alpha value for software low pass filter, value can be 0-1 */
#define ALPHA   (0.5f)


void getPedalReadings();
float get_APPS1_Pedal_Percent();
float get_APPS2_Pedal_Percent();
float get_BSE_Pedal_Percent();
unsigned int Throttle_getAPPS1SensorSum();
unsigned int Throttle_getAPPS2SensorSum();
unsigned int Throttle_getBSESensorSum();
void calculatePedalPercents();
void applyLowPassFilter();
bool check_BSE_Range_Fault();
bool check_APPS1_Range_Fault();
bool check_APPS2_Range_Fault();
bool check_10PercentAPPS_Fault();
bool check_Brake_Plausibility_Fault();


#endif /* THROTTLE_H_ */
