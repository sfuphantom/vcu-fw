/*
 * vcu.h
 *
 *  Created on: Mar 16, 2020
 *      Author: gabriel
 */

#ifndef HARDWARE_VCU_REV2_H_
#define HARDWARE_VCU_REV2_H_


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

/********* RGB STATUS LEDS ONV VCU ****************/
#define RGB_LED_PORT          hetRAM1
#define BLUE_LED              pwm1
#define GREEN_LED             pwm2
#define RED_LED               pwm3

/********* READY TO DRIVE PIEZO PWM ****************/
#define BUZZER_PORT           hetRAM1
#define READY_TO_DRIVE_BUZZER pwm0

/********* BRAKE SYSTEM ENCODER MIN/MAX ****************/
/* These allow for checking short to GND and 5V faults */
#define BSE_MIN_VALUE         409
#define BSE_MAX_VALUE         3685

/****************** BRAKE LIGHT *************************/
#define BRAKING_THRESHOLD     2000 // threshold for turning on brake light
#define BRAKE_LIGHT_PORT      gioPORTA
#define BRAKE_LIGHT_PIN       6

/*********   UART PORTS   ****************/
#define PC_UART               sciREG
#define BMS_UART              scilinREG

/*********   SPI PORTS   ****************/
#define DAC_SPI_PORT          mibspiREG1
#define HV_SPI_PORT           mibspiREG3

#endif /* HARDWARE_VCU_REV2_H_ */
