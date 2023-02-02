
#include "Phantom_sci.h"

#include <halcogen_vcu/include/sci.h>
#include <halcogen_vcu/include/gio.h>
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "board_hardware.h"


#define NUMBER_OF_SIMULATION_MESSAGES 3
static volatile uint8_t messageCounter = 0;
static volatile uint32_t serialData = 0; // there is 24 bit standard type so when we cast, we have to cast to 32 bit hence 4 bytes


void UARTSend(sciBASE_t *sci, char data[])
{
    char *first = &data[0];
    sciSend(sci, strlen(data),(uint8 *)first);
}

void UARTInit(sciBASE_t *sci, uint32 baud)
{
    _enable_IRQ();
    sciInit();
    sciSetBaudrate(sci, baud);

    sciEnableNotification(sci, SCI_RX_INT);
    sciReceive(sci, 1, (unsigned char*)NULL); // clear interrupt flag
}

void UARTprintf(const char *_format, ...)
{
   char str[128];
   memset(str, '\0', 128 * sizeof(char));
   int8_t length = -1;

   va_list argList;
   va_start( argList, _format );

   length = vsnprintf(str, sizeof(str), _format, argList);
   str[127] = '\0';

   va_end( argList );

   if (length > 0)
   {
      sciSend(PC_UART, (unsigned)length, (unsigned char*)str);
   }
}

void UARTprintln(const char *_format, ...)
{
    char str[128];
    memset(str, '\0', 128 * sizeof(char));
    int8_t length = -1;

    va_list argList;
    va_start( argList, _format );

    length = vsnprintf(str, sizeof(str), _format, argList);

    str[127] = '\0';

    va_end( argList );

    if (length > 0)
    {
        sciSend(PC_UART, (unsigned)length, (unsigned char*)str);

	    // only diff between this and UARTprintf because passing variable args between functions in C are weird and I don't wanna deal with it rn 
        sciSend(PC_UART, 2, "\r\n");
    }
}


uint32_t getSimData()
{
    while(messageCounter < NUMBER_OF_SIMULATION_MESSAGES);

    gioSetBit(gioPORTA, 5, 1);

    uint32_t ret = serialData;

    // reset volatile values
    messageCounter = 0;
    serialData = 0;

    return ret;
}

void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data)
{
//    sciSend(sci, 1, (unsigned char*) &data);

	#ifdef VCU_SIM_MODE
    if (messageCounter < NUMBER_OF_SIMULATION_MESSAGES)
    {
        serialData |= data << (messageCounter*8);

        messageCounter++;
    }
    #else
    #endif
}

