/*
 * task_logger.h
 *
 *  Created on: Feb 11, 2023
 *      Author: rafgu
 */

#ifndef PHANTOM_TASKS_HEADERS_TASK_LOGGER_H_
#define PHANTOM_TASKS_HEADERS_TASK_LOGGER_H_

#include "vcu_common.h"
#include "string.h"
#include "stdio.h"
#include "ansi_colors.h"


TaskHandle_t LoggerInit();
uint8_t Log(const char* str);
uint8_t LogColor(const char* color, const char* str);
uint8_t LogFromISR(const char* color, const char* str);
void GetLogHeader(eSource source, const char* color, char* str);


#endif /* PHANTOM_TASKS_HEADERS_TASK_LOGGER_H_ */
