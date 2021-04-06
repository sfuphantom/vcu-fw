/*
 * test_interface.c
 *
 *  Created on: Apr 6, 2021
 *      Author: Mahmoud Ahmed, Rafael Guevara
 */

#include "test_interface.h"
#include "sci.h"
#include "Phantom_sci.h"
//#include "phantom_pl455.h"
#include "string.h"
#include "stdbool.h"
//#include "sys_main.h"
#include "pl455.h"
#include <stdio.h>
#include "stdlib.h"
//#include "thermistor.h"
#include "hwConfig.h"

static unsigned char chr;
static unsigned char charbuf[BUFFER_SIZE];
static unsigned char command[BUFFER_SIZE];
static BYTE BMSArray[BMSByteArraySize*(TOTALBOARDS)];
static unsigned char getcmp[] = {'g', 'e', 't', '\0'};

//------TEST COMMANDS----------
void getAllReadings(void);
void getAllTemperatures(void);
void getSingleVoltageReading(uint8_t cell);

static volatile testBuffer testbuf = {
    0,
    0,
    charbuf,
    BUFFER_SIZE,
    0,
    sizeof(unsigned char)
};

// String input and command that is called for that input
auxcmd testAUXCommands[] =
{
 { .str = "allvoltages", .cmd = getAllReadings},
 { .str = "alltemperatures", .cmd = getAllTemperatures},
};

// For getting specific readings eg. "get voltage 4"
getcmd testGETCommands[] =
{
 { .str = "voltage", .sensor = voltage},
 { .str = "temperature", .sensor = temperature},
};

// Receives input character from serial terminal
void echoChar(void)
{
    /* Await further character */

        sciReceive(PC_UART, 1,(unsigned char *)&chr);
        processChar(chr);
}

// Processes the incoming character, if char is a return carraige(enter) then process the
// argument. If char is a backspace then delete a character from the buffer, else add the
// new character to the buffer
void processChar(unsigned char character)
{
    if(character == '\r')
    {
        argumentParse(charbuf);
        displayPrompt();
    }
    else if(character == '\b')
    {
        UARTprintf("\b");
        UARTprintf(" ");
        UARTprintf("\b");

        pop_tb();
    }
    else{
        push_tb(character);
        //sciSend(PC_UART, 1,(unsigned char *)&character);
        UARTprintf("\r> ");
        UARTprintf((char *)&testbuf.buffer[1]);
    }
}

// Pushes character to the end of the buffer
void push_tb(unsigned char character)
{
    charbuf[testbuf.count] = character;

    testbuf.tail = (testbuf.tail + 1) % testbuf.length;

    testbuf.count++;
}

// Removes character from the top of the buffer
void pop_tb(void)
{
    charbuf[testbuf.count] = '\x00';
    testbuf.count--;
}

// Displays prompt
void displayPrompt(void)
{
    UARTprintf("\n\r> ");
}


//--------AUX COMMANDS----------
// Gets all BMS readings
void getAllReadings(void)
{
    getCurrentReadings();
}

// Gets all temperature readings
void getAllTemperatures(void)
{
    uint8_t input;
    for(input=0;input<8;input++)
    {
        printThermistorReadings(input);
        UARTprintf("\n\r");
    }

    sciReceive(PC_UART, 1,(unsigned char *)&chr);
}

//--------GET COMMANDS----------
// Get a specific cell voltage from BMS
void getSingleVoltageReading(uint8_t cell)
{
    char buf[50];

    getBMSSlaveArray(BMSArray);


    uint32 tempVal = BMSArray[cell + cell-1]*16*16 + BMSArray[cell + cell];
    double div = tempVal/65535.0; //FFFF
    double fin = div * 5.0;

    UARTprintf("\n\rCell %d Voltage: %fV \n\r", cell, fin);
}

// Get a specific cell temperature
void getSingleTemperature(uint8_t cell)
{
    UARTprintf("Read cell temperature\n\r");
}

//---------ARGUMENT PARSING-----------
// Determines whether the input is a get function or an aux function. If it's a get function,
// extracts the integer argument and calls executeGETCommand. If there's no spaces, it assumes
// its an aux function and passes the command to executeAUXCommand
void argumentParse(unsigned char charArray[])
{
    unsigned char arg[10];
    unsigned char argnum[10];


    int i;
    for(i = 0; charArray[i+1] != ' ' && i < 20; i++)
    {
        command[i] = charArray[i+1];
    }

    i++;

    if(strcmp(&command[0], getcmp) == 0)
    {
        uint8_t j = 0;
        for(; charArray[i+1] != ' '; i++)
        {
            arg[j] = charArray[i+1];
            j++;
        }

        arg[j] = '\0';

        j = 0;

        for(; charArray[i+1] != '\0'; i++)
        {
            argnum[j] = charArray[i+1];
            j++;
        }

        uint16_t argint = atoi(argnum);
        executeGETCommand(arg, argint);
    }
    else
    {
        command[i] = '\0';
        executeAUXCommand(command);
    }
}

// Compares input with predefined aux functions and executes
void executeAUXCommand(unsigned char command[])
{
    uint8_t i;

    for(i=0; i<MAX_AUX; i++)
    {
        if(strcmp(&command[0], testAUXCommands[i].str) == 0)
        {
            testAUXCommands[i].cmd();
        }
    }

    if(strcmp(&command[0], "hotdog") == 0)
    {
        UARTprintf("\n\rMMMMmmmmm");
    }

    arrayCleanup();
}

// Compares input with predefined get functions and executes
void executeGETCommand(unsigned char command[], uint16_t argument)
{
    uint8_t i;
    signalIndex signalRequest;

    for(i=0; i<MAX_AUX; i++)
    {
        if(strcmp(&command[0], testGETCommands[i].str) == 0)
        {
            signalRequest = testGETCommands[i].sensor;
        }
    }

    switch(signalRequest)
    {
        case voltage:
            getSingleVoltageReading(argument);
            break;
        case temperature:
            getSingleTemperature(argument);
            break;
        default:
            break;
    }

    arrayCleanup();
}

// Cleans up arrays by setting them back to 0
void arrayCleanup(void)
{
    memset(command, 0, BUFFER_SIZE);
    memset(charbuf, 0, BUFFER_SIZE);
    testbuf.count = 1;
}




