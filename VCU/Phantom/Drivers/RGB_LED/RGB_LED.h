/*
 * RGB_LED.h
 *
 *  Created on: Mar 3, 2021
 *      Authors: Rafael Guevara, Joshua Li Guo
 */

//Halcogen Setup
//1.Enable HET Drivers
//2.Enable pwm channel outputting to specified pin
//3.Enable Output of pin in HET window

#ifndef SOURCE_RGB_H_
#define SOURCE_RGB_H_

#include <stdint.h>

/*
 * MACHINE STATE COLOR CODING
 *
 * TRACTIVE_OFF     CYAN
 * TRACTIVE_ON      MAGENTA
 * RUNNING          GREEN
 * MINOR_FAULT      YELLOW
 * SEVERE_FAULT     RED
 */

#define DEFAULT_PWM_DUTY    50

// Preset parameters for RGB_LED_drive
#define RGB_OFF     0,                  0,                  0
#define RGB_RED     DEFAULT_PWM_DUTY,   0,                  0
#define RGB_GREEN   0,                  DEFAULT_PWM_DUTY,   0
#define RGB_BLUE    0,                  0,                  DEFAULT_PWM_DUTY
#define RGB_YELLOW  DEFAULT_PWM_DUTY,   DEFAULT_PWM_DUTY,   0
#define RGB_MAGENTA DEFAULT_PWM_DUTY,   0,                  DEFAULT_PWM_DUTY
#define RGB_CYAN    0,                  DEFAULT_PWM_DUTY,   DEFAULT_PWM_DUTY
#define RGB_WHITE   DEFAULT_PWM_DUTY/2, DEFAULT_PWM_DUTY,   DEFAULT_PWM_DUTY

// Initialize the driver
void RGB_LED_Init();

// Reset driver to initial state
void RGB_LED_reset();

// Example use: RGB_LED_drive(RGB_CYAN) or RGB_LED_drive(65, 40, 25)
void RGB_LED_drive(uint32_t redLedPwm, uint32_t greenLedPwm, uint32_t blueLedPwm);

#endif /* SOURCE_RGB_H_ */
