/*
 * RTD_Buzzer.c
 *
 *  Created on: Nov 19, 2020
 *      Author: gabriel
 */
#include "RTD_Buzzer.h"

void RTD_Buzzer_Init(void)
{
    pwmSetDuty(BUZZER_PORT, READY_TO_DRIVE_BUZZER, 0); // stop the ready to drive buzzer PWM from starting automatically
}

