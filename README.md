# VCU-FW
Vehicle Control Unit - Firmware for the TMS570LS1227 written upon the freeRTOS v9 platform


# Debug Commands 

To interact with the VCU through a serial terminal, use the following numbers
on the keypad.

```  
ECHO_THROTTLE=1,
ECHO_APPS1,
ECHO_APPS2,
ECHO_BSE,
STAT_RUN,  // vTaskGetRunTimeStats
STAT_START,  // xTaskGetTickCount
TASK_LIST, // vTaskList
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