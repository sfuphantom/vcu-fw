from enum import Enum

from typing import List
from datetime import datetime

from dataclasses import dataclass

@dataclass
class DataTimePair:
    """
    Abstract container to hold data value pair
    """
    VALUE: any
    TIME: any 
@dataclass
class VCUEventsTriggers:
    """
    Data class to contain the VCU events 
    """
    # Allows user to 
    # to log all (including unchanged) events, or only log changed events
    # which will be easy to iterate over 
    #  >>> response = vcu.write(...)
    #  >>> vcu_events = response._event_triggers
    #  >>> vcu_state  = response._state
    #  >>> if logAllEvents == False:
    #  >>>    for event in vcu_events.fields():
    #  >>>        if vcu_event is not None  # This means that that it is a DataTimePair
    #  >>>             logger.log(vcu_events.event)
    #  >>> if logAllEvents == True:
    #  >>>     for event in vcu_events.field():
    #  >>>           logger.log(vcu_events.event)


    EVENT_APPS1_RANGE_FAULT : DataTimePair = None
    EVENT_APPS2_RANGE_FAULT: DataTimePair = None
    EVENT_BSE_RANGE_FAULT: DataTimePair = None
    EVENT_FP_DIFF_FAULT: DataTimePair = None
    EVENT_RESET_CAR: DataTimePair = None
    EVENT_READY_TO_DRIVE : DataTimePair = None
    EVENT_TRACTIVE_ON : DataTimePair = None
    EVENT_TRACTIVE_OFF : DataTimePair = None
    EVENT_BRAKE_PLAUSIBILITY_CLEARED : DataTimePair = None
    EVENT_BRAKE_PLAUSIBILITY_FAULT : DataTimePair = None
    EVENT_UNRESPONSIVE_APPS: DataTimePair = None


class VCUStates(Enum):
    """
    The VCU states, refer to VCU\Phantom\data_structures\vcu_common.h
    """
    TRACTIVE_OFF = 1
    TRACTIVE_ON =2 
    RUNNING = 3
    MINOR_FAULT = 4
    SEVERE_FAULT = 5


@dataclass
class ResponseVCU:
    """
    The data structure in which the VCU returns values via UART communication
    """
    EVENTS : VCUEventsTriggers
    STATE: VCUStates


if __name__ == "__main__":
    Events = VCUEventsTriggers(EVENT_APPS1_RANGE_FAULT=DataTimePair(VALUE=True, TIME = datetime.now()))
    State = VCUStates.MINOR_FAULT

    responseTest = ResponseVCU(EVENTS=Events, STATE=State)

    assert responseTest.EVENTS.EVENT_APPS1_RANGE_FAULT is not None

    assert responseTest.STATE is VCUStates.MINOR_FAULT

    print(responseTest.EVENTS.EVENT_APPS1_RANGE_FAULT.TIME)
    print(responseTest.STATE)


    



