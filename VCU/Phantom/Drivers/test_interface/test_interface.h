/*
 * test_interface.h
 *
 *  Created on: Apr 6, 2021
 *      Author: Mahmoud Ahmed, Rafael Guevara
 */

#ifndef PHANTOM_DRIVERS_TEST_INTERFACE_TEST_INTERFACE_H_
#define PHANTOM_DRIVERS_TEST_INTERFACE_TEST_INTERFACE_H_

#include "sci.h"
#include "Phantom_sci.h"

#define BUFFER_SIZE     100

typedef struct {
    uint32_t head;      // index of buffer head
    uint32_t tail;      // index of buffer head
    unsigned char *buffer;    // byte buffer to store the data
    uint32_t length;    // size of buffer
    uint32_t count;     // Current number of elements in buffer
    uint32_t datasize;  // size of each data in buffer
} testBuffer;

typedef enum {
    HV_V,
    shutdown,
    IMD,

    MAX_SIGNAL
} signalIndex;

typedef struct {
    char str[BUFFER_SIZE];
    void (*cmd)(void);
} auxcmd;

typedef struct {
    char str[BUFFER_SIZE];
    signalIndex sensor;
} getcmd;

typedef enum {
    allvoltages,
    alltemperatures,

    MAX_AUX
} auxIndex;


void echoChar(void);
void processChar(unsigned char character);
void push_tb(unsigned char character);
void pop_tb(void);
void displayPrompt(void);
void executeAUXCommand(unsigned char command[]);
void executeGETCommand(unsigned char command[], uint16_t argument);
void getSingleVoltageReading(uint8_t cell);
void getSingleTemperature(uint8_t cell);
void argumentParse(unsigned char charArray[]);
void arrayCleanup(void);

#endif /* PHANTOM_DRIVERS_TEST_INTERFACE_TEST_INTERFACE_H_ */
