
#include <halcogen_vcu/include/sci.h>
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

// int UART_printf(sciBASE_t* sciREG, char* format_str, ...)
// {
//     // gonna try something soon -Josh
//     // TODO
// }

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
}
