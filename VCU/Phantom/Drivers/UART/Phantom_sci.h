/*
 * Phantom_sc.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_PHANTOM_SCI_H_
#define INCLUDE_PHANTOM_SCI_H_

#include <halcogen_vcu/include/sci.h>

#ifdef __cplusplus
extern "C" {
#endif

#define START_SIM_DATA 's'
#define END_SIM_DATA '\n'

void UARTInit(sciBASE_t *sci, uint32 baud);
void UARTSend(sciBASE_t *sci, char data[]);
void UARTprintf(const char *_format, ...);
void UARTprintln(const char *_format, ...);

void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data);
uint32_t getSimData();


#endif /* INCLUDE_PHANTOM_SCI_H_ */
