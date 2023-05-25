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

// TODO: test that code functionality doesn't change by using packed structs
typedef struct SerialPedalData_t{
    uint32_t padding : 10; 
    uint16_t apps2 : 10 ; 
    uint16_t apps1 : 12; // LSB
    // uint16_t bse   : 12;
    // uint32_t padding : 30; // make struct size 64 (64- 36) TODO: double check this
}__attribute__((packed, aligned(1))) SerialPedalData_t;

void UARTInit(sciBASE_t *sci, uint32 baud);
void UARTSend(sciBASE_t *sci, char data[]);
void UARTprintf(const char *_format, ...);
void UARTprintln(const char *_format, ...);

void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data);
SerialPedalData_t getSerialPedalData();


#endif /* INCLUDE_PHANTOM_SCI_H_ */
