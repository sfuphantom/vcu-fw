/*
 * vcu_common.c
 *
 *  Created on: Feb 13, 2023
 *      Author: rafgu
 */

#include "vcu_common.h"
#include "Phantom_sci.h"

bool any(uint8_t num, ...)
{
	va_list valist;
	bool result = 0;

	va_start(valist, num);

	int i;
	for (i = 0; i < num; i++)
	{
		result = result || va_arg(valist, int);
	}

	va_end(valist);

	return result;
}

bool all(uint8_t num, ...)
{
	va_list valist;
	bool result = 1;

	va_start(valist, num);

	int i;
	for (i = 0; i < num; i++)
	{
		result = result && va_arg(valist, int);
	}

	va_end(valist);

	return result;
}
