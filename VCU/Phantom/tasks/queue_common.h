#ifndef PHANTOM_QUEUE_COMMON_H_
#define PHANTOM_QUEUE_COMMON_H_

/* Queue Graph

    Status Q
        length: 10 (TBD)
        item size: sizeof(int) // FAULT or RTDS or TSAL
        IN: Throttle Actor, Interrupt Actor
        OUT: State Machine

    State for Throttle Q
        length: 1
        item size: sizeof(int) // State
        IN: State Machine
        OUT: Throttle Actor

    State for Interrupt Q
        length: 1
        item size: sizeof(int) // State
        IN: State Machine
        OUT: Interrupt Actor

    // reason for two state Qs is because a Queue cannot be in multiple queue sets at the same time

    Pedal Q
        length: 1
        item size: sizeof(int) * 3 // pedalReadings
        IN: Throttle Agent
        OUT: Throttle Actor

    Interrupt Q
        length: 10 (TBD)
        item size: TBD
        IN: Interrupt Agent
        OUT: Interrupt Actor
    
    Print Q
        length: probably a lot
        item size: packet size TBD
        IN: Global
        OUT: UART task
*/

#endif