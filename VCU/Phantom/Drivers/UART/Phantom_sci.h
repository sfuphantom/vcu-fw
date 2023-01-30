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

// defined here, since everyone who cares about sim mode must need to rely on UART 
#define VCU_SIM_MODE 1  
#define START_SIM_DATA 's'
#define END_SIM_DATA '\n'

void UARTInit(sciBASE_t *sci, uint32 baud);
void UARTSend(sciBASE_t *sci, char data[]);
void UARTprintf(const char *_format, ...);
void UARTprintln(const char *_format, ...);
void split(char* str, const char* delimeter, char* buffer, int buffer_size);
void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data);
uint32_t getSimData();


#endif /* INCLUDE_PHANTOM_SCI_H_ */
