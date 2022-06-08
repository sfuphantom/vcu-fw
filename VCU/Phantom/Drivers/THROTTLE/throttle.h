// /*
//  * throttle.h
//  *
//  *  Created on: May 5, 2021
//  *      Author: jaypacamarra
//  */

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



// /* For calculating throttle padding */
// /* Padding will eliminate unintended range faults at 0% or 100% pedal presses */
 #define PADDING_PERCENT         (0.08f) // Must be between 0.0 and 1.0
 #define PADDED_BSE_MIN_VALUE    (BSE_MIN_VALUE * (1U + PADDING_PERCENT))
 #define PADDED_BSE_MAX_VALUE    (BSE_MAX_VALUE * (1U - PADDING_PERCENT))
 #define PADDED_APPS1_MIN_VALUE  (APPS1_MIN_VALUE * (1U + PADDING_PERCENT))
 #define PADDED_APPS1_MAX_VALUE  (APPS1_MAX_VALUE * (1U - PADDING_PERCENT))
 #define PADDED_APPS2_MIN_VALUE  (APPS2_MIN_VALUE * (1U + PADDING_PERCENT))
 #define PADDED_APPS2_MAX_VALUE  (APPS2_MAX_VALUE * (1U - PADDING_PERCENT))

// /* Hysteresis */
 #define HYSTERESIS      (200U)

// /* Brake Light readability */
 #define BRAKE_LIGHT_ON      0
 #define BRAKE_LIGHT_OFF     1

// /* Alpha value for software low pass filter, value can be 0-1 */
#define ALPHA   (0.5f)


//hold raw Pedal readings from adc
typedef struct RawSensorValues {       //added by kevinlitvin

    adcData_t FP_data_array[3];

    //assume compiler initializes folowing float variables to 0
    unsigned int volatile BSE_sensor_sum;
    unsigned int volatile FP_sensor_1_sum;
    unsigned int volatile FP_sensor_2_sum;


} RawSensorValues;

//hold filtered Pedal readings: percentages, difference between APPS1 & APPS2
typedef struct FilteredSensorValues {   //added by kevinlitvin


    //assume compiler initializes folowing float variables to 0
    float BSE_previous_filtered_sensor_values;
    float APPS1_previous_filtered_sensor_values;
    float APPS2_previous_filtered_sensor_values;

    float volatile Percent_APPS1_Pressed;
    float volatile Percent_APPS2_Pressed;

    //following holds the absolute value of the     
    //difference between the above two variables
    float FP_sensor_diff;

    float volatile Percent_BSE_Pressed;

} FilteredSensorValues;



// defined in another file (probably under Driver/RTOS or in tasks/source/task_throttle)
//but may be entirely removed based on data structure implentation

//NOTE: these variables are referenced in throttle.c

extern TimerHandle_t xTimers[NUMBER_OF_TIMERS];  //moved from .c to .h -kevinlitvin
extern bool APPS1_RANGE_FAULT_TIMER_EXPIRED;	 //moved from .c to .h -kevinlitvin
extern bool APPS2_RANGE_FAULT_TIMER_EXPIRED;	 //moved from .c to .h -kevinlitvin
extern bool BSE_RANGE_FAULT_TIMER_EXPIRED;       //moved from .c to .h -kevinlitvin
extern bool FP_DIFF_FAULT_TIMER_EXPIRED;		 //moved from .c to .h -kevinlitvin



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


 #endif  THROTTLE_H_ 