/*
 * phantom_can.c
 *
 *  Created on: Nov 20, 2021
 *      Author: Rafael Guevara
 */


#include "phantom_can.h"

static uint8_t tx_done;

static const uint8_t outgoing_data_ptr[9] = {'H','E','L','L','O','!','!','!','\0'};

static void sixteen_bit_to_byte(uint16* data, uint8* data_bytes){

    int n;
    for(n = 0; n+1 < sizeof(uint8); n++ ){
        data_bytes[n]   |= data[n] & 0x3;
        data_bytes[n+1] |= data[n] & 0x12;
    }


}


void phantom_can_init(){

    _enable_IRQ();
    _enable_interrupt_();
    canInit();
    vimInit();
canEnableErrorNotification(canREG1);

tx_done=false;



}

void canMessageNotification(canBASE_t *node, uint32 messageBox)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (15) */

//    if(messageBox == 0xE){

    if(node==canREG1)
         {
//          while(!canIsRxMessageArrived(canREG2, canMESSAGE_BOX14));
//          canGetData(canREG2, canMESSAGE_BOX14, rx_ptr); /* copy to RAM */
//          rx_ptr +=8;

        tx_done = true;

        UARTSend(PC_UART, "Transmitting data...\r\n");
    }


/* USER CODE END */
}
void sendVCUState(int VCU_RUNNING, int VCU_FAULT, int TS_OFF, int TS_ON)
{


    canTransmit(canREG1, canMESSAGE_BOX11, outgoing_data_ptr);

    UARTSend(PC_UART, "Transmitting data...\r\n");

//    uint16 outgoing_data[4] = { 0 }; //make it a struct
//
//    outgoing_data[0] = VCU_RUNNING;
//    outgoing_data[1] = VCU_FAULT;
//    outgoing_data[2] = TS_OFF;
//    outgoing_data[3] = TS_ON;
//
//    uint8 *outgoing_data_ptr = NULL;
//
//    sixteen_bit_to_byte(outgoing_data, outgoing_data_ptr);
//
//    int j = 0;
//    for (j = 0; j < 8; j++)
//    {
//    canTransmit(canREG1, canMESSAGE_BOX11, outgoing_data_ptr);
//    while(!tx_done);              /* ... wait until transmit request is through        */
//          tx_done=false;
//
//        outgoing_data_ptr += 8; /* next chunk ...*/
//    }
}


void delayms(uint16_t ms) {
      volatile unsigned int delayval;
      delayval = ms * 8400;   // 8400 are about 1ms
      while(delayval--);
}


