/*
 * stateMachineTaskTest.h
 *
 *  Created on: May 15, 2021
 *      Author: junaidkhan
 */

#ifndef PHANTOM_TEST_STATEMACHINE_STATEMACHINETASKTEST_H_
#define PHANTOM_TEST_STATEMACHINE_STATEMACHINETASKTEST_H_

#include "vcu_data.h"
#include "FreeRTOS.h"
#include "os_semphr.h"

extern SemaphoreHandle_t vcuKey;
extern data* VCUDataPtr;
void stateMachineTaskTest(void* parameters);


#endif /* PHANTOM_TEST_STATEMACHINE_STATEMACHINETASKTEST_H_ */
