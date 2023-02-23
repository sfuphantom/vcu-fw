/*
 * board_hardware.h
 *
 *  Created on: Jun 10, 2020
 *      Author: gabriel
 */

#ifndef PHANTOM_HARDWARE_LAUNCHPAD_HW_BOARD_HARDWARE_H_
#define PHANTOM_HARDWARE_LAUNCHPAD_HW_BOARD_HARDWARE_H_

#include "adc.h"
#include "reg_het.h"


/*********   READY TO DRIVE GPIO   ****************/
#define READY_TO_DRIVE_PORT   gioPORTA
#define READY_TO_DRIVE_PIN    2

/*********   SHUTDOWN CIRCUIT GPIO   ****************/
#define SHUTDOWN_CIRCUIT_PORT gioPORTA
#define BMS_FAULT_PIN         5 // needs to be confirmed
#define IMD_FAULT_PIN         6 // needs to be confirmed
#define BSPD_FAULT_PIN        7 // needs to be confirmed

/***************  TSAL GPIO  *********************/
#define TSAL_PORT             hetPORT1
#define TSAL_ACTIVE_PIN       5 // needs to be confirmed

/***************  WATCHDOG GPIO  *********************/
#define WATCHDOG_PORT         hetPORT1
#define WATCHDOG_PIN          2

/***************  WATCHDOG GPIO  *********************/
#define TASK_TIMING_PORT      hetPORT1
/********* RGB STATUS LEDS ON VCU ****************/
#define RGB_LED_PORT          hetRAM1
#define BLUE_LED              pwm1
#define GREEN_LED             pwm2
#define RED_LED               pwm3

/********* READY TO DRIVE PIEZO PWM ****************/
#define BUZZER_PORT           hetRAM1
#define READY_TO_DRIVE_BUZZER pwm0

/********* BRAKE SYSTEM ENCODER MIN/MAX ****************/
/* These allow for checking short to GND and 5V faults */
#define BSE_MIN_VALUE           1268
#define BSE_MAX_VALUE           3686
#define APPS1_MIN_VALUE         1268
#define APPS1_MAX_VALUE         3686
#define APPS2_MIN_VALUE         410
#define APPS2_MAX_VALUE         1229

/****************** BRAKE LIGHT *************************/
#define BRAKING_THRESHOLD     1500 // threshold for turning on brake light
#define BRAKE_LIGHT_PORT      hetPORT1
#define BRAKE_LIGHT_PIN       12
#define BRAKE_LIGHT BRAKE_LIGHT_PORT, BRAKE_LIGHT_PIN

/*********   UART PORTS   ****************/
#define PC_UART               scilinREG
#define BMS_UART              sciREG

/*********   SPI PORTS   ****************/
#define DAC_SPI_PORT          mibspiREG1
#define HV_SPI_PORT           mibspiREG3

/*********   I2C PORTS   ****************/
#define LV_MONITOR_I2C_PORT   i2cREG1

/*------------------------------------------------------------------------------------------------------------------*/

/*********************************************************************************
 *                          GLOBAL VARIABLES I NEED TO CLEAN UP
 *********************************************************************************/


/*********************************************************************************
 *                          DEBUG PRINTING DEFINES
 *********************************************************************************/
#define TASK_PRINT  0
#define STATE_PRINT 0
#define APPS_PRINT  0 // if this is enabled, it hogs the whole cpu since the task it runs in is called every 10ms and is the highest priority. doesn't allow other tasks/interrupts to run
#define BSE_PRINT   0 // if this is enabled, it hogs the whole cpu since the task it runs in is called every 10ms and is the highest priority. doesn't allow other tasks/interrupts to run


//xQueueHandle xq;

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
//typedef enum {TRACTIVE_OFF, TRACTIVE_ON, RUNNING, FAULT} State;


#endif /* PHANTOM_HARDWARE_LAUNCHPAD_HW_BOARD_HARDWARE_H_ */
