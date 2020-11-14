/*
 * Current_transducer.c
 *
 *  Created on: Sep 3, 2020
 *      Author: yashv
 */

#include "Current_transducer.h"
#include "sci.h"
#include "adc.h"
#include "stdlib.h"

double voltage_to_current()
{
    adcData_t adc_data;   // ADC data structure
    adcData_t *adc_data_ptr = &adc_data;  // ADC data pointer
    unsigned int NumberOfChars1, value; // declare variables

    sciInit();   // Initialize the SCI(UART)module
    adcInit();   // Initialize the ADC module

    adcStartConversion(adcREG1, 1U);  // Start ADC conversion

    while(!adcIsConversionComplete(adcREG1, 1U)); //Wait for ADC conversion
    adcGetData(adcREG1, 1U, adc_data_ptr); //Store conversion into ADC pointer
    value = (unsigned int)adc_data_ptr->value;
    sensor_voltage = ((value/adc_resolution)*max_voltage);

   // if(voltage < Uref){     // negative current
   // if(voltage > Uref){     // positive current
    output_current  = (sensor_voltage - Uref)*I_PN/1.25 ;

    return output_current;
}

// Fault check method
bool isHVcurrent_inRange()
{
    flag = false;

    if((output_current >=  MAX_POS_CURRENT) || (output_current <= MIN_NEGATIVE_CURRENT))
               flag =  true;

    return flag;
}
