from enum import Enum

from typing import List, Union

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
    TIME: int

@dataclass
class StateData:
    STATE: VCUStates
    TIME: Union[int, None]

class ResponseVCU:
    """
    The data structure in which the VCU returns values via UART communication
    after sending a sequence of values

    Allows for the same events to be stored if triggered at different times
    It is necessary for the this class to specify the VCU state
    
    Stores an array of events and their time of trigger
    Stores the state of the VCU and the time of change (if changed)
    """

    def __init__(self, state: VCUStates):
        self._events: List[EventData] = []
        self._state : StateData = StateData(state, None)
    
    def add_event(self, event_name: VCUEvents, event_time: int):
        """
        Add an event to the list of events

        :param event_name: a VCU event
        :param event_time: the relative time of the event
        """

        #check that the event exists in the available VCU Events enum
        if not isinstance(event_name, VCUEvents):
            raise EventError(f"Invalid event type {event_name}, should be of type {VCUEvents.__name__}")
        
        event_data = EventData(EVENT=event_name,TIME=event_time)
        
        if not self._check_duplicate(event_data):
            raise EventError(f"Duplicate event : {event_name} at time {event_time}")
    
        self._events.append(event_data)


    def set_state(self, state: VCUStates, state_time_trigger: int = None):
        """
        Specifify the state of the VCU and the time of state change (if changed)

        :param state: the current state of the VCU
        :param state_time_trigger: relative time of state change, None if state was unchanged
        """
        if not isinstance(state, VCUStates):
            raise StateError(f"Invalid state {state}, should be of type {VCUStates.__name__}")
        
        self._state = StateData(STATE=state, TIME=state_time_trigger)

    def _check_duplicate(self, event_data: EventData) -> bool:
        """
        Check that there does not already exist an event
        with the same time and type

        :param event_data: the event data to compare to the existing events
        :return: returns true if no duplicate events are given
        """
        for existing_event in self.events:
            if existing_event.EVENT == event_data.EVENT \
               and existing_event.TIME == event_data.TIME:
                  return False
        return True

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

    


    



