/*
 * task_sensor_read.h
 *
 *  Created on: Apr 16, 2020
 *      Author: gabriel
 */

#ifndef PHANTOM_TASKS_TASK_SENSOR_READ_H_
#define PHANTOM_TASKS_TASK_SENSOR_READ_H_

#include "IMD.h"

void vSensorReadTask(void *);

//Helper Functions
void setIMDStates(IMDData_t);


#endif /* PHANTOM_TASKS_TASK_SENSOR_READ_H_ */
