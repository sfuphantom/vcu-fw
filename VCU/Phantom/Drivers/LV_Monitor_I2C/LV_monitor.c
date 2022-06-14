#include "LV_monitor.h"
#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)

//#include "sys_common.h"
//#include "i2c.h"
//#include "het.h"
//#include "gio.h"
//#include "stdlib.h"
//#include "sci.h"

void lv_monitorInit(){

    uint8_t LV_Calibration[4] = {0x14,0x00}; //this is a calculated value 5120->0x1400

/* USER CODE BEGIN (3) */

    //Testing SCI
//    _enable_IRQ();
    /* Initialize SCI module */
//    sciInit();
//    gioInit();
    i2cInit();

//    gioSetBit(gioPORTA,6,1);

    // int repeat = 0; int delay =1000000;
    // int i = 0;
    //     for (i = 0; i < delay; i++);


    /* I2C Init as per GUI
     *  Mode = Master - Transmitter
     *  baud rate = 100KHz
     *  Count = 10
     *  Bit Count = 8bit
     */


    /*****************************************/
    //// Setting Calibration values
    /*****************************************/

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(LV_MONITOR_I2C_PORT, LV_Slave_Address);

    /* Set direction to Transmitter */
    /* Note: Optional - It is done in Init */
    i2cSetDirection(LV_MONITOR_I2C_PORT, I2C_TRANSMITTER);

    /* Configure Data count */
    /* Slave address + Word address write operation before reading */
    i2cSetCount(LV_MONITOR_I2C_PORT,3);

    /* Set mode as Master */
    i2cSetMode(LV_MONITOR_I2C_PORT, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(LV_MONITOR_I2C_PORT);

    /* Transmit Start Condition */
    i2cSetStart(LV_MONITOR_I2C_PORT);

    /* Send the Word Address */
    i2cSendByte(LV_MONITOR_I2C_PORT, LV_calibration_register);

    i2cSend(LV_MONITOR_I2C_PORT,2,LV_Calibration);
    //i2cSend(i2cBASE_t *i2c, uint32 length, uint8 * data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(LV_MONITOR_I2C_PORT) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(LV_MONITOR_I2C_PORT) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(LV_MONITOR_I2C_PORT);

    //while(i2cIsMasterReady(LV_MONITOR_I2C_PORT) != true);
}

int LV_reading(uint16_t mode){

    while(i2cIsMasterReady(LV_MONITOR_I2C_PORT) != true);

    uint8_t RX_Data_Master1[2]; //to hold bits

    i2cSetSlaveAdd(LV_MONITOR_I2C_PORT, LV_Slave_Address);

    /* Set direction to Transmitter */
    /* Note: Optional - It is done in Init */
    i2cSetDirection(LV_MONITOR_I2C_PORT, I2C_TRANSMITTER);

    /* Configure Data count */
    /* Slave address + Word address write operation before reading */
    i2cSetCount(LV_MONITOR_I2C_PORT,1);

    /* Set mode as Master */
    i2cSetMode(LV_MONITOR_I2C_PORT, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(LV_MONITOR_I2C_PORT);

    /* Transmit Start Condition */
    i2cSetStart(LV_MONITOR_I2C_PORT);

    /* Send the Word Address */
    i2cSendByte(LV_MONITOR_I2C_PORT, mode);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(LV_MONITOR_I2C_PORT) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(LV_MONITOR_I2C_PORT) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(LV_MONITOR_I2C_PORT);

    /*****************************************/
    //// Start receving the data From Slave
    /*****************************************/

    /* wait until MST bit gets cleared, this takes
     * few cycles after Bus Busy is cleared */
    while(i2cIsMasterReady(LV_MONITOR_I2C_PORT) != true);

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(LV_MONITOR_I2C_PORT, LV_Slave_Address);

    /* Set direction to receiver */
    i2cSetDirection(LV_MONITOR_I2C_PORT, I2C_RECEIVER);

    /* Configure Data count */
    /* Note: Optional - It is done in Init, unless user want to change */
    i2cSetCount(LV_MONITOR_I2C_PORT, LV_DATA_COUNT); //only need to receive 1 value

    /* Set mode as Master */
    i2cSetMode(LV_MONITOR_I2C_PORT, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(LV_MONITOR_I2C_PORT);

    /* Transmit Start Condition */
    i2cSetStart(LV_MONITOR_I2C_PORT);

    /* Tranmit LV_DATA_COUNT number of data in Polling mode */
    i2cReceive(LV_MONITOR_I2C_PORT, LV_DATA_COUNT, RX_Data_Master1); //location of stored data
    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(LV_MONITOR_I2C_PORT) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(LV_MONITOR_I2C_PORT) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(LV_MONITOR_I2C_PORT);


    uint16_t MSB_data = RX_Data_Master1[0];
    uint16_t LSB_data = RX_Data_Master1[1];
    MSB_data <<=8;
    uint16_t MSBnLSB_data = MSB_data | LSB_data;


     // * Shunt Voltage *2.5uV   => Voltage(mV)
     // * Bus Voltage   *1.25mv  => Voltage (V)
     // * Current       *1mA     => Current(A)
     // * Power         *25mW    => Watt(W)

    if(mode == LV_bus_voltage_register){
        MSBnLSB_data *= 1.25;
    }
    else if(mode == LV_current_register){
        MSBnLSB_data *= 1;
    }
    else if(mode == LV_Shunt_register){
         MSBnLSB_data *= 2.5;
    }
    else if(mode == LV_power_register){
        MSBnLSB_data *= 25;
    }
    else{
        MSBnLSB_data = 0;
    }

    return MSBnLSB_data;
}
