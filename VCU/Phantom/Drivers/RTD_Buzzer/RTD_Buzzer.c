/*
 * RTD_Buzzer.c
 *
 *  Created on: Nov 19, 2020
 *      Author: gabriel
 */
#include "RTD_Buzzer.h"

void RTD_Buzzer_Init(void)
{
    pwmStop(BUZZER_PORT, READY_TO_DRIVE_BUZZER); // stop the ready to drive buzzer PWM from starting automatically
}

