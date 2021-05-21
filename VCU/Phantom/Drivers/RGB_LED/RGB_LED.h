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

#define RGB_OFF     0x00
#define RGB_RED     0x01
#define RGB_GREEN   0x02
#define RGB_BLUE    0x04
#define RGB_YELLOW  0x03
#define RGB_MAGENTA 0x05
#define RGB_CYAN    0x06
#define RGB_WHITE   0x07

#define DEFAULT_PWM_DUTY    25
#define DEFAULT_COLOR       RGB_WHITE

// Initialize the driver
void RGB_LED_Init();

// Reset driver to initial state
void RGB_LED_reset();

// colorCode is any of the RGB color macros above
void RGB_LED_drive(uint8_t colorCode, uint32_t pwm);

#endif /* SOURCE_RGB_H_ */
