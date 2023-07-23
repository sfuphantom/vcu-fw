from enum import Enum

from typing import List
import time

from dataclasses import dataclass

@dataclass
class DataTimePair:
    VALUE: any
    TIME: any #TODO: @raf update so that TIME


@dataclass
class EventsTriggers:
    """
    Store events in a data class where if an event happened
    then store
    """
    # Allow for the opportunity to allow user for the top
    # to log unchanged events, or only log changed events
    # which will be easy to iterate over 
    #  >>> response = vcu.write(...)
    #  >>> vcu_events = response._event_triggers
    #  >>> vcu_state  = response._state
    #  >>> if logAllEvents() == False:
    #  >>>    for event in vcu_events.fields():
    #  >>>        if vcu_event is not None  # This means that that it is a DataTimePair
    #  >>>             logger.log(vcu_events.event)
    #  >>>    if logAllEvents == False:
    #  >>>         for event in vcu_events.field():
    #  >>>              logger.log(vcu_events.event)
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

if __name__ == "__main__":
    x = EventsTriggers(EVENT_APPS1_RANGE_FAULT=DataTimePair(VALUE=True, TIME = time.gmtime))
    print(x)



