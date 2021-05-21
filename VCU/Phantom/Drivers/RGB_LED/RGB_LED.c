/*
 * RGB_LED.c
 *
 *  Created on: Mar 3, 2021
 *      Authors: Joshua Li Guo
 */
#include "gio.h"
#include "het.h"
#include "board_hardware.h"

#include "RGB_LED.h"

#if !defined(RGB_LED_PORT) || !defined(RED_LED) || !defined(GREEN_LED) || !defined(BLUE_LED)

#define RGB_LED_PORT          hetRAM1
#define BLUE_LED              pwm1
#define GREEN_LED             pwm2
#define RED_LED               pwm3

#endif

#define DEFAULT_PWM_PERIOD_US   10.0
#define DEFAULT_SIGNAL          (hetSIGNAL_t) {DEFAULT_PWM_DUTY, DEFAULT_PWM_PERIOD_US}

// This function must be called before using
// anything else in the module.
void RGB_LED_Init()
{
    pwmSetSignal(RGB_LED_PORT, RED_LED, DEFAULT_SIGNAL);
    pwmSetSignal(RGB_LED_PORT, GREEN_LED, DEFAULT_SIGNAL);
    pwmSetSignal(RGB_LED_PORT, BLUE_LED, DEFAULT_SIGNAL);

    pwmStart(RGB_LED_PORT, RED_LED);
    pwmStart(RGB_LED_PORT, GREEN_LED);
    pwmStart(RGB_LED_PORT, BLUE_LED);

    RGB_LED_drive(DEFAULT_COLOR, DEFAULT_PWM_DUTY);
}

void RGB_LED_reset()
{
    ASSERT(isInitialized);
    
    pwmStop(RGB_LED_PORT, RED_LED);
    pwmStop(RGB_LED_PORT, GREEN_LED);
    pwmStop(RGB_LED_PORT, BLUE_LED);

    pwmSetSignal(RGB_LED_PORT, RED_LED, DEFAULT_SIGNAL);
    pwmSetSignal(RGB_LED_PORT, GREEN_LED, DEFAULT_SIGNAL);
    pwmSetSignal(RGB_LED_PORT, BLUE_LED, DEFAULT_SIGNAL);

    pwmStart(RGB_LED_PORT, RED_LED);
    pwmStart(RGB_LED_PORT, GREEN_LED);
    pwmStart(RGB_LED_PORT, BLUE_LED);

    RGB_LED_drive(DEFAULT_COLOR, DEFAULT_PWM_DUTY);
}

void RGB_LED_drive(uint8_t colorCode, uint32_t pwm)
{
    ASSERT(isInitialized);

    pwmSetDuty(RGB_LED_PORT, RED_LED, pwm * ((colorCode & RGB_RED) && 1) + 1);
    pwmSetDuty(RGB_LED_PORT, GREEN_LED, pwm * ((colorCode & RGB_GREEN) && 1) + 1);
    pwmSetDuty(RGB_LED_PORT, BLUE_LED, pwm * ((colorCode & RGB_BLUE) && 1) + 1);
}
