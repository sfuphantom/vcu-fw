
#include "Phantom_sci.h"

#include <halcogen_vcu/include/sci.h>
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "board_hardware.h"

static unsigned char serialData[128];
static uint8_t beginProcessing = 0;
static uint8_t endProcessing = 0;
static uint8_t index = 0;
static unsigned char data;
static uint8_t rx_flag = 0;

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

unsigned char* getSimData(int i)
{
    while(!endProcessing);

    endProcessing = 0;
    return serialData;
}

void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data)
{
    sciSend(sci, 1, (unsigned char*) &data);
    if (data == START_SIM_DATA && !endProcessing || beginProcessing)
    {
        if (beginProcessing)
        {
            serialData[index] = data;

            if (data == '\n')
            {
                endProcessing = 1;
                beginProcessing = 0;
            }

            index++;
        }
        else
        {
            beginProcessing = 1;
            index = 0;
        }

    }
}


void split(char* str, const char* delimeter, char* buffer, int buffer_size){
    /*
    Args:
        str: original string to split
        delimeter: delimeter
        buffer: string to copy contents to
        buffer_size: size of buffer
    Return:
        No explicit returns. Output of function is copied to buffer
    */

    // Returns first token
    char *token = strtok(str, delimeter);

    // Keep printing tokens while one of the
    // delimiters present in str[].
    int index = 0;
    while (token != NULL && index < buffer_size)
    {
        buffer[index] = *token;
        token = strtok(NULL, delimeter);
        index++;
    }
}

