# VCU-FW
Vehicle Control Unit - Firmware for the TMS570LS1227 written upon the freeRTOS v9 platform

## Releases 

Releases are read-only branches that express major milestones in development. They have been through some form of testing and are known to be stable. If you are not developing for the VCU, please use one of these branches.

|                                Release                                         |                           Information                       |    Date     |
|                                  ---                                           |                               ---                           |     ---     |
|[PhantomRelease1](https://github.com/sfuphantom/vcu-fw/tree/VCUPhantomRelease1) |[Pull request](https://github.com/sfuphantom/vcu-fw/pull/39) |  2023-03-02 |

## Debug Commands 

To interact with the VCU through a serial terminal (460800 baud rate), use the following numbers
on the keypad.

```  
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
```
### (1,2,3,4) Echo the value of Throttle, APPS1/APPS2, and BSE respectfully
```
33%
```
### (6) View live scheduler tick count 
```
1029894
```
### (7) View run time statistics for each task  
```
Task          State   Prio    Stack    Num
********************************************
VCU_CLI         R       0       314     1
IDLE            R       0       226     3
ThrottleTask    B       4       84      2
Tmr Svc         B       1       474     4
```
