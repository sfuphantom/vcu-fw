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

void UARTInit(sciBASE_t *sci, uint32 baud);
void UARTSend(sciBASE_t *sci, char data[]);
void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data);

#endif /* INCLUDE_PHANTOM_SCI_H_ */
