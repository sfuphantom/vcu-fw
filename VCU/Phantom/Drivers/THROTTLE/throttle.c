/*
 * throttle.c
 *
 *  Created on: May 5, 2021
 *      Author: jaypacamarra
 */

#include "throttle.h"

adcData_t FP_data[3];
adcData_t *FP_data_ptr = FP_data;
unsigned int volatile BSE_sensor_sum;   // = 0;
unsigned int volatile FP_sensor_1_sum;  // = 0;
unsigned int volatile FP_sensor_2_sum;  // = 0;

float volatile Percent_APPS1_Pressed;
float volatile Percent_APPS2_Pressed;
float volatile Percent_BSE_Pressed;

float BSE_filtered_sensor_value;                 // filtered BSE sensor value - jaypacamarra
float BSE_previous_filtered_sensor_values = 0;   // previous BSE filtered output - jaypacamarra
float APPS1_filtered_sensor_value;               // filtered APPS1 sensor value - jaypacamarra
float APPS1_previous_filtered_sensor_values = 0; // previous APPS1 filtered output - jaypacamarra
float APPS2_filtered_sensor_value;               // filtered APPS2 sensor value - jaypacamarra
float APPS2_previous_filtered_sensor_values = 0; // previous BSE filtered output - jaypacamarra

bool is_there_10DIFF_fault;
bool is_there_BSE_range_fault;
bool is_there_APPS1_range_fault;
bool is_there_APPS2_range_fault;
bool is_there_brake_plausibility_fault;

uint32_t volatile fault_10DIFF_counter_ms = 0;      // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_BSE_Range_counter_ms = 0;            // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_APPS1_Range_counter_ms = 0;          // hold duration of fault in milliseconds - jaypacamarra
uint32_t fault_APPS2_Range_counter_ms = 0;          // hold duration of fault in milliseconds - jaypacamarra

extern TimerHandle_t xTimers[NUMBER_OF_TIMERS];

bool APPS1_range_fault_timer_started = false;
bool APPS2_range_fault_timer_started = false;
bool BSE_range_fault_timer_started   = false;
bool FP_diff_fault_timer_started     = false;

extern bool APPS1_RANGE_FAULT_TIMER_EXPIRED;   //jaypacamarra
extern bool APPS2_RANGE_FAULT_TIMER_EXPIRED;   //jaypacamarra
extern bool BSE_RANGE_FAULT_TIMER_EXPIRED;     //jaypacamarra
extern bool FP_DIFF_FAULT_TIMER_EXPIRED;       //jaypacamarra

float FP_sensor_diff;


/** @fn void getPedalReadings(void)
*   @brief Reads brake, APPS1, and APPS2 sensors
*   @Return This function does not return anything, it only
*           updates the variables:
*               - BSE_sensor_sum
*               - FP_sensor_1_sum
*               - FP_sensor_2_sum
*/
void getPedalReadings() {
    adcStartConversion(adcREG1, adcGROUP1);
    while (!adcIsConversionComplete(adcREG1, adcGROUP1));
    adcGetData(adcREG1, adcGROUP1, FP_data_ptr);
    BSE_sensor_sum = (unsigned int)FP_data[0].value;   // BSE
    FP_sensor_2_sum = (unsigned int)FP_data[1].value;  // APPS2
    FP_sensor_1_sum = (unsigned int)FP_data[2].value;  // APPS1

    // Update pedal percentages
    calculatePedalPercents();
}

/** @fn unsigned int Throttle_getAPPS1SensorSum(void)
*   @brief Getter for APPS1 ADC sensor reading value
*   @Return unsigned int
*/
unsigned int Throttle_getAPPS1SensorSum() {
    return FP_sensor_1_sum;
}

/** @fn unsigned int Throttle_getAPPS2SensorSum(void)
*   @brief Getter for APPS2 ADC sensor reading value
*   @Return unsigned int
*/
unsigned int Throttle_getAPPS2SensorSum() {
    return FP_sensor_2_sum;
}

/** @fn unsigned int Throttle_getBSESensorSum(void)
*   @brief Getter for BSE ADC sensor reading value
*   @Return unsigned int
*/
unsigned int Throttle_getBSESensorSum() {
    return BSE_sensor_sum;
}

/** @fn float get_APPS1_Pedal_Percent(void)
*   @brief Getter for APPS1 pedal percent
*   @Return pedal percent as float
*/
float get_APPS1_Pedal_Percent() {
    return Percent_APPS1_Pressed;
}

/** @fn float get_APPS2_Pedal_Percent(void)
*   @brief Getter for APPS2 pedal percent
*   @Return pedal percent as float
*/
float get_APPS2_Pedal_Percent() {
    return Percent_APPS2_Pressed;
}

/** @fn float get_BSE_Pedal_Percent(void)
*   @brief Getter for BSE pedal percent
*   @Return pedal percent as float
*/
float get_BSE_Pedal_Percent() {
    return Percent_BSE_Pressed;
}

/** @fn void calculatePedalPercents(void)
*   @brief Calculates the percent pressed of the brake pedal
*          and the accelerator pedal
*   @Return This function does not return anything, it only
*           updates the VCU data structure
*/
void calculatePedalPercents() {
    // APPS1
    if(FP_sensor_1_sum < PADDED_APPS1_MIN_VALUE)
        Percent_APPS1_Pressed = 0;
    else if(FP_sensor_1_sum > PADDED_APPS1_MAX_VALUE)
        Percent_APPS1_Pressed = 1.0;
    else
        Percent_APPS1_Pressed = (FP_sensor_1_sum - PADDED_APPS1_MIN_VALUE) / (float)(PADDED_APPS1_MAX_VALUE - PADDED_APPS1_MIN_VALUE);

    // APPS2
    if(FP_sensor_2_sum < PADDED_APPS2_MIN_VALUE)
        Percent_APPS2_Pressed = 0;
    else if(FP_sensor_2_sum > PADDED_APPS2_MAX_VALUE)
        Percent_APPS2_Pressed = 1.0;
    else
        Percent_APPS2_Pressed = (FP_sensor_2_sum - PADDED_APPS2_MIN_VALUE) / (float)(PADDED_APPS2_MAX_VALUE - PADDED_APPS2_MIN_VALUE);

    // BSE
    if(BSE_sensor_sum < PADDED_BSE_MIN_VALUE)
        Percent_BSE_Pressed = 0;
    else if(BSE_sensor_sum > PADDED_BSE_MAX_VALUE)
        Percent_BSE_Pressed = 1.0;
    else
        Percent_BSE_Pressed = (BSE_sensor_sum - PADDED_BSE_MIN_VALUE) / (float)(PADDED_BSE_MAX_VALUE - PADDED_BSE_MIN_VALUE);
}

/** @fn void applyLowPassFilter(void)
*   @brief Applies a software low pass filter to the pedal signals.
*   @note  alpha must be between 0 and 1.
*           - smaller alpha means smoother signal but slower
*           - bigger alpha means less smooth signal but faster
*   @Return This function does not return anything, it only
*           updates the variables:
*               - BSE_sensor_sum
*               - FP_sensor_1_sum
*               - FP_sensor_2_sum
*/
void applyLowPassFilter() {
    // Filter the raw BSE,APPS1, and APPS2 sensor values
    BSE_sensor_sum = BSE_previous_filtered_sensor_values + ALPHA * (BSE_sensor_sum - BSE_previous_filtered_sensor_values);
    FP_sensor_1_sum = APPS1_previous_filtered_sensor_values + ALPHA * (FP_sensor_1_sum - APPS1_previous_filtered_sensor_values);
    FP_sensor_2_sum = APPS2_previous_filtered_sensor_values + ALPHA * (FP_sensor_2_sum - APPS2_previous_filtered_sensor_values);

    // Set previous filtered values to current filtered values
    BSE_previous_filtered_sensor_values = BSE_sensor_sum;
    APPS1_previous_filtered_sensor_values = FP_sensor_1_sum;
    APPS2_previous_filtered_sensor_values = FP_sensor_2_sum;
}

/** @fn bool check_BSE_Range_Fault(void)
*   @brief Checks if the BSE is in the right voltage range
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
bool check_BSE_Range_Fault() {
    if (BSE_sensor_sum < BSE_MIN_VALUE) // BSE is assumed shorted to GND
    {
        if(BSE_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[4], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                BSE_range_fault_timer_started = true;
            }
        }
        if(BSE_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_BSE_range_fault = true; // Set fault flag in vcu data structure
        }
        else
        {
            is_there_BSE_range_fault = false; // Set fault flag in vcu data structure
        }
    }
    else if (BSE_sensor_sum > BSE_MAX_VALUE) // BSE is assumed shorted to VCC
    {
        if(BSE_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[4], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                BSE_range_fault_timer_started = true;
            }
        }

        if(BSE_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_BSE_range_fault = true; // Set fault flag in vcu data structure
        }
        else
        {
            is_there_BSE_range_fault = false; // Set fault flag in vcu data structure
        }
    }
    else
    {
        // should be in normal range
        is_there_BSE_range_fault = false;

        // Stop fault timer
        xTimerStop(xTimers[4], portMAX_DELAY);
        BSE_range_fault_timer_started = false;
        BSE_RANGE_FAULT_TIMER_EXPIRED = false;
    }

    return is_there_BSE_range_fault;
}

/** @fn bool check_APPS1_Range_Fault(void)
*   @brief Checks if the APPS1 sensor is in the right voltage range
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
bool check_APPS1_Range_Fault() {
    if (FP_sensor_1_sum < APPS1_MIN_VALUE) // APPS1 is assumed shorted to GND
    {

        if(APPS1_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[2], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                APPS1_range_fault_timer_started = true;
            }
        }

        if(APPS1_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_APPS1_range_fault = true; // apps1 range fault occured
        }
        else
        {
            is_there_APPS1_range_fault = false; // no fault
        }

    }
    else if (FP_sensor_1_sum > APPS1_MAX_VALUE) // APPS1 is assumed shorted to VCC
    {

        if(APPS1_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[2], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                APPS1_range_fault_timer_started = true;
            }
        }

        if(APPS1_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_APPS1_range_fault = true; // apps1 range fault occured
        }
        else
        {
            is_there_APPS1_range_fault = false; // no fault
        }
    }
    else
    {
        // should be in normal range so no fault
        is_there_APPS1_range_fault = false;

        // Stop fault timer
        xTimerStop(xTimers[2], portMAX_DELAY);
        APPS1_range_fault_timer_started = false;
        APPS1_RANGE_FAULT_TIMER_EXPIRED = false;
    }

    return is_there_APPS1_range_fault;
}

/** @fn bool check_APPS2_Range_Fault(void)
*   @brief Checks if the APPS2 sensor is in the right voltage range
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
bool check_APPS2_Range_Fault() {
    if (FP_sensor_2_sum < APPS2_MIN_VALUE) // APPS2 assumed shorted to GND
    {
        if(APPS2_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[3], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                APPS2_range_fault_timer_started = true;
            }
        }

        if(APPS2_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_APPS2_range_fault = true; // Set fault flag in vcu data structure
        }
        else
        {
            is_there_APPS2_range_fault = false; // no fault
        }
    }
    else if (FP_sensor_2_sum > APPS2_MAX_VALUE) // APPS2 assumed shorted to VCC
    {
        if(APPS2_range_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[3], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                APPS2_range_fault_timer_started = true;
            }
        }

        if(APPS2_RANGE_FAULT_TIMER_EXPIRED)
        {
            is_there_APPS2_range_fault = true; // Set fault flag in vcu data structure
        }
        else
        {
            is_there_APPS2_range_fault = false; // no fault
        }
    }
    else
    {
        // should be in normal range so no fault
        is_there_APPS2_range_fault = false;

        // Stop fault timer
        xTimerStop(xTimers[3], portMAX_DELAY);
        APPS2_range_fault_timer_started = false;
        APPS2_RANGE_FAULT_TIMER_EXPIRED = false;
    }

    return is_there_APPS2_range_fault;
}

/** @fn bool check10PercentAPPS(void)
*   @brief Checks if APPS1 and APPS2 are within 10% of each other.
*          A fault means they are not within 10% of each other.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
bool check_10PercentAPPS_Fault() {
    FP_sensor_diff = fabs(Percent_APPS2_Pressed - Percent_APPS1_Pressed); // Calculate absolute difference between APPS1 and APPS2 readings

    if (FP_sensor_diff > 0.10)
    {
        if(FP_diff_fault_timer_started == false)
        {
            if(xTimerStart(xTimers[5], pdMS_TO_TICKS(50)) != pdPASS) // start software timer for apps1 range fault
            {
                for(;;);
            }
            else
            {
                FP_diff_fault_timer_started = true;
            }
        }

        if(FP_DIFF_FAULT_TIMER_EXPIRED)
        {
            // Fault
            is_there_10DIFF_fault = true;
        }
    }
    else
    {
        // Stop the fault timer
        xTimerStop(xTimers[5], portMAX_DELAY);
        FP_diff_fault_timer_started = false;
        FP_DIFF_FAULT_TIMER_EXPIRED = false;

        // No fault
        is_there_10DIFF_fault = false;
    }

    return is_there_10DIFF_fault;
}

/** @fn bool check_Brake_Plausibility_Fault(void)
*   @brief Checks if the brakes and accelerator are pressed at the same time.
*          A fault means the brake is pressed and the APPS reads 25% or higher.
*   @Return This function returns:
*           True -> Fault
*           False -> No Fault
*/
bool check_Brake_Plausibility_Fault() {
    if (BSE_sensor_sum >= BRAKING_THRESHOLD + HYSTERESIS &&
            Percent_APPS1_Pressed >= 0.25 &&
            Percent_APPS2_Pressed >= 0.25)
    {
        // Set fault
        is_there_brake_plausibility_fault = true;

    }
    else
    {
        // APPS/Brake plausibility fault only clears if APPS returns to less than 5% pedal position
        // with or without brake operation (see EV.5.7.2) - jaypacamarra
        if (Percent_APPS1_Pressed < 0.05 &&
                Percent_APPS2_Pressed < 0.05)
        {
            // No fault
            is_there_brake_plausibility_fault = false;
        }
    }

    return is_there_brake_plausibility_fault;
}