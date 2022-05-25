develop - BRANCH 

 

 

Changes in: vcu-fw/VCU/Phantom/Drivers/RTOS/phantom_freertos.c 

 

Move following 4 variable definitions (lines 15-18) into throttle.c  file: 

bool APPS1_RANGE_FAULT_TIMER_EXPIRED; 

bool APPS2_RANGE_FAULT_TIMER_EXPIRED; 

bool BSE_RANGE_FAULT_TIMER_EXPIRED; 

bool FPE_DIFF_FAULT_TIMER_EXPIRED; 

 

Reason: 

Currently, extern declarations (not the same as definition) for these variables are in throttle.c, which can cause linkage error during compilation. If we move all variables into the throttle.c file, the variables’ scope is limited to only one file, and so compilation will reduce likelihood of linkage errors. (Translation unit needs to fetch from less files). 

 

As a result,  

 

Move following 4 function definitions (lines 379-397) into throttle.c file: 

 

void APPS1_SEVERE_FAULT_CALLBACK(TimerHandle_t xTimers); 

void APPS2_SEVERE_FAULT_CALLBACK (TimerHandle_t xTimers); 

void BSE_SEVERE_FAULT_CALLBACK (TimerHandle_t xTimers); 

void FPE_ SEVERE _FAULT_ CALLBACK (TimerHandle_t xTimers); 

 

Also move the related function declarations from the phantom_freertos.h into throttle.h 

 

Reason:  

These functions change the previously mentioned variables, so we have to all move the functions into the same scope as where the variables are defined. Since phantom_freertos.c already contains throttle.h, there won’t be a problem in accessing the functions if we move them.  

 

Overall, this change makes the code a lot more readable since now with the changes, the variables’ definition/declations aren’t scatted in different files in the vcu-fw repository. 

 

 

 

 

 

 

 

 

 

 

Changes in: vcu-fw/VCU/Phantom/Drivers/THROTTLE/throttle.c 

 

(1) Remove unnecessary extern declarations from the 4 moved variables. 

(2) Move all variables inside the functions which use them to limit their scope to local 

Or 

    Create structs to to hold variables: 

Time struct  

Bool_fault struct 

Raw_sensor_value struct 

Filtered_sensor_value struct 

 

 

 

Pending decisions:  

Still need to decide where to move the variable: 

 

 extern TimerHandle_t Xtimers[NUMBER_OF_TIMERS];  

 

Because this variable may not be linked with the definition from phantom_freertos.c during compilation 

 

Note:  

 

declaration in vcu-fw/VCU/Phantom/tasks/source/task_throttle.c, but not used inside the file: 

 

extern TimerHandle_t Xtimers[NUMBER_OF_TIMERS];   
