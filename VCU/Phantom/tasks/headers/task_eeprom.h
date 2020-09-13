/*
 * task_eeprom.h
 *
 *  Created on: Aug 9, 2020
 *      Author: junaidkhan
 */

#ifndef PHANTOM_TASKS_HEADERS_TASK_EEPROM_H_
#define PHANTOM_TASKS_HEADERS_TASK_EEPROM_H_

#include "FreeRTOS.h"
#include "os_task.h"
#include "sci.h"
#include "string.h"
#include "os_semphr.h"
#include "stdio.h"

#include "eeprom_driver.h"
#include "vcu_data.h"
#include "F021.h"
#include "board_hardware.h"
#include "Phantom_sci.h"


extern TaskHandle_t eepromHandler;
extern SemaphoreHandle_t vcuKey;
extern SemaphoreHandle_t powerfailureFlagKey;
extern data VCUData;
extern void *pVCUDataStructure;   // Need to define this in main as (void *)pVCUDataStructure = &VCUData
extern uint8_t powerFailureFlag;  // Need to define this main.c

void vEeprom(void *p);

#endif /* PHANTOM_TASKS_HEADERS_TASK_EEPROM_H_ */
