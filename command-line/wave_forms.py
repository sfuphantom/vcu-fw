
from typing import Union
from abc import ABC, abstractmethod
from enum import Enum
import math
import random

from vcu_simulation import (
	APPS1_MIN,
	APPS1_MAX,
	APPS2_MIN,
	APPS2_MAX,
	BSE_MIN,
	BSE_MAX
)

class VCU_Pedals(Enum):
    APPS1 = 1,
    APPS2 = 2,
    BSE = 3

class VCU_Pedal:

    max_voltages: dict[VCU_Pedals, int] = {
        VCU_Pedals.APPS1: APPS1_MAX,
        VCU_Pedals.APPS2: APPS2_MAX,
        VCU_Pedals.BSE: BSE_MAX,
    }

    min_voltages : dict[VCU_Pedals, int] = {
        VCU_Pedals.APPS1: APPS1_MIN,
        VCU_Pedals.APPS2: APPS2_MIN,
        VCU_Pedals.BSE: BSE_MIN,
    }

    def __init__(self, pedal_type: VCU_Pedals):
        """
        Create an to encapsulate the pedal type, min voltage, and max voltage
        """
        self._pedal_type: VCU_Pedals = pedal_type
        self._min: int = self.min_voltages[pedal_type]
        self._max: int  = self.max_voltages[pedal_type]

class AnalogWave(ABC):
    """
    Abstract class to lay foundation for waveforms
    """
    _registered_waves: dict[str, object] = {}

    @classmethod
    def register(cls, identifier=None):
        def decorator(subclass):
            nonlocal identifier
            if identifier is None:
                identifier = subclass.__name__
            if identifier in cls._registered_waves:
                raise KeyError(f"Identifier {identifier} already registered with {subclass}")
            cls._registered_waves[identifier] = subclass
            return subclass
        return decorator
    
    @classmethod
    @abstractmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        pass
    
    @classmethod
    @abstractmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        pass
    
    @classmethod
    def map_percentage_to_voltage(cls, pedal_spec: VCU_Pedal, percentage: int) -> float:
        return percentage * (pedal_spec._max - pedal_spec._min) + pedal_spec._min
    
    @classmethod
    def set_values(cls, pedal_spec: VCU_Pedal, percent_pressed: int, vcu_values: dict[VCU_Pedals, int]):
        scaled_percentage = cls.map_percentage_to_voltage(pedal_spec, percent_pressed)
        vcu_values[pedal_spec._pedal_type] = scaled_percentage
    

        
@AnalogWave.register("SH")
class HalfSinusodialWave(AnalogWave):
    """
    Sinusodial Wave used to simulate a car when turning a corner
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return math.sin(percent_pressed * math.pi)


    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return -math.sin(percent_pressed * math.pi) + 1

@AnalogWave.register("SF")
class FullSinusodialWave(AnalogWave):
    """
    Full Sinusodiaul Wave
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return (-(math.cos(percent_pressed *2 * math.pi)) + 1)/2


    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return -math.sin(percent_pressed * math.pi) + 1 


@AnalogWave.register("T")    
class TriangularWave(AnalogWave):
    """
    Linear mapping
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return percent_pressed
    
    @classmethod
    def inverse_mapping(cls ,percent_pressed: float) -> float:
        return 1-percent_pressed
    
@AnalogWave.register("R")    
class RandomWave(AnalogWave):
    """
    Random wav
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return random.uniform(0, percent_pressed)
    
    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return random.uniform(0, 1- percent_pressed)
    
@AnalogWave.register("M")    
class MaxWave(AnalogWave):
    """
    Maps to the maximum of pedals voltage, used for stress testing
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return 1
    
    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return 0
    
@AnalogWave.register("O")    
class MinWave(AnalogWave):
    """
    Maps to the minimum of a pedal's voltage, used for excluding a pedal's role in the simulation
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return 0
    
    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return 1

@AnalogWave.register("P") 
class SpikeWave(AnalogWave):
    """
    Used to measure circuit fauls for spikes in pedal voltage readings
    """

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return 1 if percent_pressed == 0 else 0
    
    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return 1 if percent_pressed != 0 else 0
    
@AnalogWave.register("I") 
class InverseWave(AnalogWave):
    "Reserved for the inverse"

    @classmethod
    def standard_mapping(cls, percent_pressed: float) -> float:
        return super().standard_mapping(percent_pressed)
    
    @classmethod
    def inverse_mapping(cls, percent_pressed: float) -> float:
        return super().inverse_mapping(percent_pressed)