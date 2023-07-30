from enum import Enum

from typing import List, Union
from datetime import datetime

from dataclasses import dataclass


class VCUEvents (Enum):
    """
    The VCU Events, refer to VCU\Phantom\data_structures\vcu_common.h
    """
    EVENT_APPS1_RANGE_FAULT = 1
    EVENT_APPS2_RANGE_FAULT = 2
    EVENT_BSE_RANGE_FAULT = 3
    EVENT_FP_DIFF_FAULT = 4
    EVENT_RESET_CAR = 5
    EVENT_READY_TO_DRIVE = 6
    EVENT_TRACTIVE_ON = 7
    EVENT_TRACTIVE_OFF = 8 
    EVENT_BRAKE_PLAUSIBILITY_CLEARED = 9
    EVENT_BRAKE_PLAUSIBILITY_FAULT = 10
    EVENT_UNRESPONSIVE_APPS = 11
class VCUStates(Enum):
    """
    The VCU states, refer to VCU\Phantom\data_structures\vcu_common.h
    """
    TRACTIVE_OFF = 0
    TRACTIVE_ON = 1 
    RUNNING = 2
    MINOR_FAULT = 3
    SEVERE_FAULT = 4

@dataclass
class EventData:
    EVENT: VCUEvents
    TIME: datetime.time

@dataclass
class StateData:
    STATE: VCUStates
    TIME: Union[datetime.time, None]

class ResponseVCU:
    """
    The data structure in which the VCU returns values via UART communication
    """

    def __init__(self):
        self._events: List[EventData] = []
        self._state : StateData = None
    
    def add_event(self, event_name: VCUEvents, event_time: datetime.time):
        """
        Add an event to the list of events

        :param event_name: a VCU event
        :param event_time: the relative time of the event
        """

        #check that the event exists in the available VCU Events enum
        if not isinstance(event_name, VCUEvents):
            raise EventError(f"Invalid event type {event_name.__class__}, should be of type {VCUEvents.__name__}")
        
        #check for duplicate events
        if event_name in self.events:
            raise EventError(f"Duplicate event : {event_name.value}")
        
        event_data = EventData(EVENT=event_name,TIME=event_time)
        self._events.append(event_data)

    
    def set_state(self, state: VCUStates, state_time_trigger: datetime.time = None):
        """
        Specifify the state of the VCU and the time of state change (if changed)

        :param state: the current state of the VCU
        :param state_time_trigger: time of state change, None if state was unchanged
        """
        if not isinstance(state, VCUStates):
            raise StateError(f"Invalid state {state}, should be of type {VCUStates.__name__}")
        
        self._state = StateData(STATE=state, TIME=state_time_trigger)
    
    @property
    def events(self) -> List[EventData]:
        return self._events
    @property
    def state(self) -> StateData:
        return self._state

class EventError(Exception):
    """
    Exceptions for VCU events
    """

class StateError(Exception):
    """
    Exceptions for VCU states 
    """

if __name__ == "__main__":
    # Events = VCUEventsTriggers(EVENT_APPS1_RANGE_FAULT=DataTimePair(VALUE=True, TIME = datetime.now()))
    # State = VCUStates.MINOR_FAULT

    # responseTest = ResponseVCU(EVENTS=Events, STATE=State)

    # assert responseTest.EVENTS.EVENT_APPS1_RANGE_FAULT is not None

    # assert responseTest.STATE is VCUStates.MINOR_FAULT

    # print(responseTest.EVENTS.EVENT_APPS1_RANGE_FAULT.TIME)
    # print(responseTest.STATE)

    


    



