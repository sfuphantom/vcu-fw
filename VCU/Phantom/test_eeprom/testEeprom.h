/*
 * testEeprom.h
 *
 *  Created on: Mar 7, 2021
 *      Author: junaidkhan
 */

#ifndef PHANTOM_TEST_EEPROM_TESTEEPROM_H_
#define PHANTOM_TEST_EEPROM_TESTEEPROM_H_

#include "vcu_data.h"
#include "FreeRTOS.h"
#include "os_semphr.h"

extern SemaphoreHandle_t vcuKey;
extern data* VCUDataPtr;
void testEeprom(void *p);


#endif /* PHANTOM_TEST_EEPROM_TESTEEPROM_H_ */
