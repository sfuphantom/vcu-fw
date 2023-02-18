
#include "Phantom_sci.h"

#include <halcogen_vcu/include/sci.h>
#include <halcogen_vcu/include/gio.h>
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "board_hardware.h"
#include "FreeRTOS.h"
#include "os_task.h"

#include "task_logger.h"
#include "task_event_handler.h"
#include "state_machine.h"

#define NUMBER_OF_SIMULATION_MESSAGES 3
static volatile uint8_t messageCounter = 0;
static volatile uint32_t serialData = 0; // there is 24 bit standard type so when we cast, we have to cast to 32 bit hence 4 bytes

#define TASK_LIST_SIZE 512

enum eCommands{
    ECHO_THROTTLE='1',
    ECHO_APPS1='2',
    ECHO_APPS2='3',
    ECHO_BSE='4',
    STAT_RUN='5',  // vTaskGetRunTimeStats
    STAT_START='6',  // xTaskGetTickCount
    TASK_LIST='7', // vTaskList
    RESET_CAR='r',
    START_ENGINE='s',
    TURN_TRACTIVE_ON='o',
};


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

void GetRuntimeStatistics(void* x)
{
    // TODO: not configured!
    // vTaskGetRunTimeStats(ptrTaskList);

    char ptrTaskList[TASK_LIST_SIZE];
    vTaskList(ptrTaskList);

    UARTSend(PC_UART, ptrTaskList);
    UARTprintln("\r");
}

void sciReceiveCallback(sciBASE_t *sci, uint32 flags, uint8 data)
{

	#ifdef VCU_SIM_MODE
    if (messageCounter < NUMBER_OF_SIMULATION_MESSAGES)
    {
        serialData |= data << (messageCounter*8);

        messageCounter++;
    }
    #else

    char buffer[16];

    switch(data){

        case TASK_LIST:
        	HandleToBack(GetRuntimeStatistics, 0, FROM_ISR);
            break;

        case ECHO_THROTTLE:
        	LogFromISR(UWHT, "No data available yet.");

            break;
        case ECHO_APPS1:
        	LogFromISR(UWHT, "No data available yet.");
            break;
        case ECHO_APPS2:
        	LogFromISR(UWHT, "No data available yet.");
            break;
        case ECHO_BSE:
        	LogFromISR(UWHT, "No data available yet.");
            break;
        case STAT_START:            
        {
            snprintf(buffer, 16, "%dms", xTaskGetTickCountFromISR());

            LogFromISR(UWHT, buffer);
            break;
        }
        case TURN_TRACTIVE_ON:
        {
            NotifyStateMachineFromISR(EVENT_TRACTIVE_ON);

            break;
        }
        case START_ENGINE:
        {
            NotifyStateMachineFromISR(EVENT_READY_TO_DRIVE);

            break;
        }
        case RESET_CAR:
        {
            NotifyStateMachineFromISR(EVENT_RESET_CAR);

            break;
        }
        default:
        {
            LogFromISR(UWHT, "Unknown: ");

            break;
        }
    }

    #endif
}



