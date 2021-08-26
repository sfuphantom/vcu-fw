/*
 * phantom_task.h
 *
 *  Created on: Aug. 25, 2021
 *      Author: Josh Guo
 */
#ifndef PHANTOM_TASK_H_
#define PHANTOM_TASK_H_

#include "hal_stdtypes.h"
#include "os_projdefs.h"

int Phantom_createTask(TaskFunction_t taskFnPtr,
					   char* const taskName,
					   uint16 stackSize,
					   void* const fnParameters,
					   uint32 taskPriority);

#endif