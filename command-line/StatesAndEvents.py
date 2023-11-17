#Author : Kevin Litvin
#Date : November 2023
#Description : Defines the data structure to organize the VCU's firmware response to simuations

from enum import Enum

from typing import List, Union

from dataclasses import dataclass

import logging




class VCUEvents (Enum):
    """
    The VCU Events, refer to VCU\Phantom\data_structures\vcu_common.h
    """

    # BEGIN_OF_EVENTS=0, Corresponding enum from the above referenced header file.
    #                    NOTE: Unimplemented here because it is not a valid event
    #                          that could (or should) be raised by the VCU   
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

    # END_OF_EVENTS again, implemeted in the header file, 
    #                      but we do not implement it as it is not a valid event

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
    TIME: float

    def __hash__(self):
        return hash((self.EVENT,self.TIME))

@dataclass
class StateData:
    STATE: VCUStates
    TIME: Union[float, None]

class ResponseVCU:
    """
    The data structure in which the VCU returns values via UART communication
    after sending a sequence of values

    Allows for the same events to be stored if triggered at different times
    It is necessary for the this class to specify the VCU state
    
    Stores the raw string parsed from the VCU
    Stores the set of events and their time of trigger
    Stores the state of the VCU and the time of change (if changed)
    """

    logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    filename='VCU_response.log',
    )

    logger = logging.getLogger("data_log")

    def __init__(self, raw_response: str):
        
        self._raw_response: str = raw_response
        self._events: set[EventData] = set() 
        self._state : StateData = None # TODO: Currently does not check intermediate states.
        self.parse_str()
        # Unclear if we want to keep track of multiple VCU state changes 
        # >>> self._state: set[StateData]  = set()  
    
    def parse_str(self):
        """
        Parse the raw string from the vcu response, and parse into events and states
        Generally, the form of the string follows the form :

        [Trigger:{:.2f}] NEW TRIGGER : Enum\n
        Ex:
        >>> "[State:5.61] NEW STATE: 2\n
        >>>  [Event:5.71] NEW EVENT: 11\n
        >>>  [Event:100032.23] NEW EVENT: 1\n"
        """
        
        for line in self._raw_response.split('\n'):

            #ignore empty string due to split formatting
            if line == "": continue
        
            #parse the time; always 2 decimal string float
            split_line = line.split(":")

            try:
                relative_time_ms = round(float(split_line[1][0:split_line[1].find(".")+3]),2)

                #the last element of the split will contain the vcu trigger
                enumeration_trigger = int(line.split(':')[2])
                if 'NEW EVENT' in line:
                    self.add_event(VCUEvents(enumeration_trigger), relative_time_ms) 
                elif 'NEW STATE' in line:
                    self.set_state(VCUStates(enumeration_trigger), relative_time_ms)
                else:
                    logging.log(logging.DEBUG , f"Unknown response : {line}")
            except Exception as e:
                print(f"Caught exception for {line}")
                logging.log(logging.INFO, f"Error from parsing : {line} - {e}")

    def add_event(self, event_name: VCUEvents, event_time: float):
        """
        Add an event to set of events

        :param event_name: a VCU event
        :param event_time: the relative time of the event trigger
        """

        #check that the event exists in the available VCU Events enum
        if not isinstance(event_name, VCUEvents):
            raise EventError(f"Invalid event type {event_name}, should be of type {VCUEvents.__name__}")
        
        event_data = EventData(EVENT=event_name,TIME=event_time)
        
        if event_data in self._events:
            raise EventError(f"Duplicate event : {event_name} at time {event_time}")
    
        self._events.add(event_data)


    def set_state(self, state: VCUStates, state_time_trigger: float = None):
        """
        Specifify the state of the VCU and the time of state change (if changed)

        :param state: the current state of the VCU
        :param state_time_trigger: relative time of state change, None if state was unchanged
        """
        if not isinstance(state, VCUStates):
            raise StateError(f"Invalid state {state}, should be of type {VCUStates.__name__}")
        
        self._state = StateData(STATE=state, TIME=state_time_trigger)

    def sorted_events(self, sort_by_type: bool = False, reversed: bool = False):
        """
        Get a sorted array of events ordered by type or time, in ascending/descending order

        :param sort_by_type: set True sort events by event type, default False for sorting by time
        :param reversed: set True to sort in descending order, default Fase
        :return: the list of events sorted by the given parameters
        """

        event_list = list(self.events)

        if sort_by_type:
            event_list.sort(key = lambda eventData: eventData.EVENT.value, reverse=reversed)
        else:
            event_list.sort(key = lambda eventData: eventData.TIME, reverse=reversed)

        return event_list
    
    def __str__(self) -> str:
        return self._raw_response
    
    def _parse_event_data(self, event_set: set[EventData]) -> str: 
        """
        Parse the event set into a string used for data storage readable
        by team members 
        """
        parsed_data = []
        for event in event_set:
            event_name = event.EVENT.name
            time_ms = int(event.TIME)
            formatted_event = f"{event_name}: {time_ms}ms"
            parsed_data.append(formatted_event)
        return "\n".join(parsed_data)
    
    def _format_state_data(self, state_data: StateData) -> str:
        if state_data is None:
            #Can change to unchanged or implement a cached value if we
            #choose to display the current VCU state rather than reporting
            #state transitions
            return ""
        state_name = state_data.STATE.name
        time_ms = int(state_data.TIME) if state_data.TIME is not None else "Unchanged"
        formatted_state = f"{state_name} - {time_ms}ms"
        return formatted_state
        
    @property
    def events(self) -> List[EventData]:
        return self._events
    
    @property
    def events_str(self) -> str:
        return self._parse_event_data(self.events)

    @property
    def state(self) -> StateData:
        return self._state
    
    @property
    def state_str(self) -> str:
        return self._format_state_data(self.state)

class EventError(Exception):
    """
    Exceptions for VCU events
    """

class StateError(Exception):
    """
    Exceptions for VCU states 
    """
