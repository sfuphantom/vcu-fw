/*
 * RGB_LED.c
 *
 *  Created on: Nov 19, 2020
 *      Author: gabriel
 */

#include "RGB_LED.h"


void RGB_LED_Init(void)
{
    // turn off RGB LEDs
    pwmStart(RGB_LED_PORT, BLUE_LED); // blue
    pwmStart(RGB_LED_PORT, GREEN_LED); // green
    pwmStart(RGB_LED_PORT, RED_LED); // red
//     maybe this can be changed in halcogen?

//     initialize HET pins ALL to output.. may need to change this later
    gioSetDirection(hetPORT1, 0xFFFFFFFF);
}

