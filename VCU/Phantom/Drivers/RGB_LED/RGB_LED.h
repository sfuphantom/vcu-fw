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

#include "hal_stdtypes.h"

/*
 * MACHINE STATE COLOR CODING
 *
 * TRACTIVE_OFF     CYAN
 * TRACTIVE_ON      MAGENTA
 * RUNNING          GREEN
 * MINOR_FAULT      YELLOW
 * SEVERE_FAULT     RED
 */

typedef struct {
    uint8 red;
    uint8 green;
    uint8 blue;
} Color;

#define DEFAULT_PWM_DUTY    50

// Preset colors
#define RGB_OFF     ((Color) { 0,  0,  0})
#define RGB_RED     ((Color) {DEFAULT_PWM_DUTY,  0,  0})
#define RGB_GREEN   ((Color) { 0, DEFAULT_PWM_DUTY,  0})
#define RGB_BLUE    ((Color) { 0,  0, DEFAULT_PWM_DUTY})
#define RGB_YELLOW  ((Color) {DEFAULT_PWM_DUTY, DEFAULT_PWM_DUTY,  0})
#define RGB_MAGENTA ((Color) {DEFAULT_PWM_DUTY,  0, DEFAULT_PWM_DUTY})
#define RGB_CYAN    ((Color) { 0, DEFAULT_PWM_DUTY, DEFAULT_PWM_DUTY})
#define RGB_WHITE   ((Color) {DEFAULT_PWM_DUTY/2, DEFAULT_PWM_DUTY, DEFAULT_PWM_DUTY})

// Initialize the driver
void RGB_init();

// Reset driver to initial state
void RGB_reset();

// Example use: RGB_drive(RGB_CYAN) or RGB_drive(65, 40, 25)
void RGB_drive(Color color);

#endif /* SOURCE_RGB_H_ */
