CODE REFACTOR OVERVIEW: 

 

Non-Boolean variables in throttle.c will be split into two structs:   

    RawSensorValues 

    FilteredSensorValues  

NOTE: Accessing throttle variables will be through structs. However, getter functions for pedal readings and faults remain the same.  



CHANGES: 

 

(1) Variables placed in created struct: RawSensorValue  

    adcData_t FP_data_array[3] 

    unsigned int volatile BSE_sensor_sum; 

    unsigned int volatile FP_sensor_1_sum; 

    unsigned int volatile FP_sensor_2_sum; 

 

(2) Variables placed in created struct: FilteredSensorValue 

    float BSE_previous_filtered_sensor_values; 

    float APPS1_previous_filtered_sensor_values; 

    float APPS2_previous_filtered_sensor_values; 

 

    float volatile Percent_APPS1_Pressed; 

    float volatile Percent_APPS2_Pressed; 

    float FP_sensor_diff;  

    float volatile Percent_BSE_Pressed; 

(3) Merged: FP_data_ptr & FP_data ->  FP_data_array 

-This way we won’t need will to initialize 	FP_data_ptr value to FP_data once a RawSensorValues variable is defined 
(or else compiler initializes FP_data_ptr to null, which would create problems when passing structs through the queue pipeline).  

-C arrays are already pointers, so we remove any ambiguity. 

 

 

 

(4) Moved 5 extern declarations into throttle.h 

    extern TimerHandle_t xTimers[NUMBER_OF_TIMERS];   

    extern bool APPS1_RANGE_FAULT_TIMER_EXPIRED;	 

    extern bool APPS2_RANGE_FAULT_TIMER_EXPIRED;	  

    extern bool BSE_RANGE_FAULT_TIMER_EXPIRED;        

    extern bool FP_DIFF_FAULT_TIMER_EXPIRED;		 

 

 (5) Removed 7 unused variable declarations/definitions 

    float BSE_filtered_sensor_value; 

    float APPS1_filtered_sensor_value; 

    float APPS2_filtered_sensor_value; 

 

    uint32_t volatile fault_10DIFF_counter_ms = 0; 

    uint32_t fault_BSE_Range_counter_ms = 0; 

    uint32_t fault_APPS1_Range_counter_ms = 0; 

    uint32_t fault_APPS2_Range_counter_ms = 0; 

 

(6) Moved 5 variable definitions from the global scope into their respective local function scope 

    bool is_there_10DIFF_fault; 

    bool is_there_BSE_range_fault; 

    bool is_there_APPS1_range_fault; 

    bool is_there_APPS2_range_fault; 

    bool is_there_brake_plausibility_fault; 
