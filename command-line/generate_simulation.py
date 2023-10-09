#Author: kevinl03 

import random
import argparse
import math
import csv
import time
import PointPlotting


CSV_FILE_NAME = "SimulatedValues.csv"
#---------------------------------------------------------------------------------#
#THESE VARIABLES CAN BE CHANGED DEPENDING ON PEDAL/VCU SPECIFICATIONS
#Most up to date specifications found here :
# https://docs.google.com/document/d/1PsZSJR6la_u_oXmN2lo8tfRcU6E8GrMdpH35ESl7d4U/

APPS1maxVoltageReading = 4.5
APPS1minVoltageReading = 1.50

APPS2maxVoltageReading = 1.5
APPS2minVoltageReading = 0.5

BSEmaxVoltageReading = 4.5
BSEminVoltageReading = 1.50

#APPSMaxPedalAngle = 225
#APPSMinPedalAngle = 180

#BSEMaxPedalAngle = 135
#BSEMinPedalAngle = -74

#--------------------------------------------------------------------------------#


#Simple linear mapping, may need to refacotr if the Power Team's design is non-linear
def mapPercentageToVoltage(percentPressed, maxVoltage, minVoltage):
    return percentPressed * (maxVoltage - minVoltage) + minVoltage

#Sin wave used for to simulate a car going around a turn where
def sinWave(PedalType, PercentPressed, Values):
    SinPercentage = math.sin(PercentPressed * math.pi)
    Values[PedalType] = mapPercentageToVoltage(SinPercentage, maxVoltage[PedalType], minVoltage[PedalType])

#Takes the value of percent pressed and reflected over the line y = 0.5
def inverseSinWave(PedalType, PercentPressed, Values):
    SinPercentage = -math.sin(PercentPressed * math.pi) + 1
    Values[PedalType] = mapPercentageToVoltage(SinPercentage, maxVoltage[PedalType], minVoltage[PedalType])

#Linearly increasing wave
def triangularWave(PedalType, PercentPressed, Values):
    Values[PedalType] = mapPercentageToVoltage(PercentPressed, maxVoltage[PedalType], minVoltage[PedalType])

#Linearaly increasing wave with discontinuity
#to test firmware for voltage change discrepencies
def scatteredWave(PedalType, PercentPressed, Values):
    randomPercentPressed = random.uniform(0, PercentPressed)
    Values[PedalType] = mapPercentageToVoltage(randomPercentPressed, maxVoltage[PedalType], minVoltage[PedalType])

#Stress testing them max voltage reading on the system
def maxWave(PedalType, PercentPressed, Values):
    Values[PedalType] = maxVoltage[PedalType]

#Exclude a pedal's role in the simulation
def minWave(PedalType, PercentPressed, Values):
    Values[PedalType] = minVoltage[PedalType]

#Create 1 spike per wave form. Precision will control how
#values seperate each spike, and cycles will determine the total
#amount of spikes per simulation
def spikeWave(PedalType, PercentPressed, Values):
    if PercentPressed == 0:
        Values[PedalType] = maxVoltage[PedalType]
    else:
        Values[PedalType] = minVoltage[PedalType]

def matchInverse(APPSWaveForm, curpercentage, Values):
    if APPSWaveForm == "S":
        inverseSinWave("BSE", curpercentage, Values)
    elif APPSWaveForm == "T":
        triangularWave("BSE", 1 - curpercentage, Values)
    elif APPSWaveForm == "R":
        scatteredWave("BSE", 1 - curpercentage, Values)
    elif APPSWaveForm == "P":
        spikeWave("BSE", 1 - curpercentage, Values)
    elif APPSWaveForm == "M":
        Values["BSE"] = BSEminVoltageReading
    elif APPSWaveForm == "O":
        Values["BSE"] = BSEmaxVoltageReading
    else:
        raise ValueError("Invalid APPSWaveForm for BSE")

# Create a dictionary to map waveform names to functions
waveform_functions = {
    "S": sinWave,
    "T": triangularWave,
    "R": scatteredWave,
    "P": spikeWave,
    "M": maxWave,
    "O": minWave,
}

# Define the maximum and minimum voltage for each PedalType
maxVoltage = {
    "APPS1": APPS1maxVoltageReading,
    "APPS2": APPS2maxVoltageReading,
    "BSE": BSEmaxVoltageReading,
}

minVoltage = {
    "APPS1": APPS1minVoltageReading,
    "APPS2": APPS2minVoltageReading,
    "BSE": BSEminVoltageReading,
}

def generateWaveform(PedalType, PercentPressed, Values, WaveForm):
    if WaveForm in waveform_functions:
        waveform_functions[WaveForm](PedalType, PercentPressed, Values)
    else:
        print(f"Unknown waveform: {WaveForm}")

#Takes the value of percent pressed and reflected over the line y = 0.5
#Such that the percent is 1-(sinwave(percentpressed)

def percentToInverseSinWave(percent):

    HorizontalScale = math.pi

    return (-math.sin(percent*HorizontalScale)+1)


def checkArguments(args):

    cycles = args.Cycles
    precision = args.Precision

    BSEWaveForm = args.BSEWaveForm
    APPSWaveForm = args.APPSWaveForm

    APPSWaves = waveform_functions.keys()
    BSEWaves = [key for key in waveform_functions.keys()] + ["I"]
    errorfound = False

    if cycles < 1:
        print("cycles must be at least 1")
        errorfound = True
    if precision < 3:
        print("precision must be at least 3")
        errorfound = True
    if APPSWaveForm not in APPSWaves:
        print(f"{APPSWaveForm} is not an available waveform to model the APPS")
        errorfound = True
    if BSEWaveForm not in BSEWaves:
        print(f"{BSEWaveForm} is not an available waveform to model the BSE")
        errorfound = True

    if errorfound:
        print("Call the script with arguments -h for more information about correct calling")
        exit()


def format_values(VCU_values: dict):
    # Format the value with 3 decimal places
    for key, value in VCU_values.items():
        formatted_value = f"{value:.3f}" 
        VCU_values[key] = formatted_value

def has_data_in_csv(file_path):
    try:
        with open(file_path, 'r', newline='') as csv_file:
            csv_reader = csv.reader(csv_file)
            # Check if the file has any rows
            for row in csv_reader:
                if row:
                    return True  # Data found, return True
            return False  # No data found, return False

    except Exception as e:
        print(f"An error occurred: {e}")
        return False  # Error occurred, return False

if __name__ == "__main__":
    my_parser = argparse.ArgumentParser(description='Arbitrary Wave Form Generator')

    # Create arguments when calling the script
    my_parser.add_argument('Cycles', metavar='Cycles', type=int,
                            help='Range: int >= 1')
    my_parser.add_argument('Precision', metavar='Precision', type=int,
                            help='The amount of values to represent each waveform. Higher precision results in a more accurate simulation for the waveform.')
    my_parser.add_argument('APPSWaveForm', metavar='APPSWaveForm', type=str, 
                           help='T = triangular, S = sinusoidal, R = Random, P = Spike, M = Max voltage, O = Min voltage')
    my_parser.add_argument('BSEWaveForm', metavar='BSEWaveForm', type=str, 
                           help='T = triangular, S = sinusoidal, R = Random, P = Spike, M = Max voltage, O = Min voltage, I = Inverse of APPS')

    # Read values from the command line/terminal
    args = my_parser.parse_args()
    checkArguments(args)

    # Placeholder for values [APPS1, APPS2, BSE]
    # Will be changed based on the arguments and the current percentage pressed
    VCU_Values = {"APPS1": 0, "APPS2": 0, "BSE": 0}
    VCU_plot_values = {key: [] for key in VCU_Values}


    #if the file is empty
    if not has_data_in_csv(CSV_FILE_NAME):
        with open(CSV_FILE_NAME, 'w', newline='') as file:
            #Write the header
            csv.writer(file).writerow([key for key in VCU_Values.keys()])

    # Pass newline='' as an argument to avoid spaces between Excel rows
    with open(CSV_FILE_NAME, 'a', newline='') as f:

        for cycle in range(args.Cycles):
            # Total number of values to represent the chosen wave
            for increment in range(args.Precision + 1):
                # NOTE: Percentage always represented as a number [0, 1] not [0, 100]
                curpercentage = increment / args.Precision

                # Sets values [APPS1, APPS2, BSE]
                generateWaveform("APPS1", curpercentage, VCU_Values, args.APPSWaveForm)
                generateWaveform("APPS2", curpercentage, VCU_Values, args.APPSWaveForm)
                generateWaveform("BSE", curpercentage, VCU_Values, args.BSEWaveForm) if args.BSEWaveForm != "I" else matchInverse(args.APPSWaveForm,curpercentage,VCU_Values)

                format_values(VCU_values=VCU_Values)
                # Writes the set of values to the CSV file
                csv.writer(f).writerow([value for value in VCU_Values.values()])
                
                for key in VCU_Values.keys():
                    VCU_plot_values[key].append(float(VCU_Values[key]))

                             
    PointPlotting.generate_VCU_plot(VCU_plot_values)
    #Throttle_Value_Simulator.sendValsFromFile('SimulatedValues.csv') #uncomment this line to send values to VCU

    



from typing import Union
from abc import ABC, abstractmethod
import re
from enum import Enum
class VCU_Pedals(Enum):
    APPS1 = 1,
    APPS2 = 2,
    BSE = 3

class VCU_Pedal:

    max_voltages: dict[VCU_Pedals, int] = {
        VCU_Pedals.APPS1: APPS1maxVoltageReading,
        VCU_Pedals.APPS2: APPS2maxVoltageReading,
        VCU_Pedals.BSE: BSEmaxVoltageReading,
    }

    min_voltages : dict[VCU_Pedals, int] = {
        VCU_Pedals.APPS1: APPS1minVoltageReading,
        VCU_Pedals.APPS2: APPS2minVoltageReading,
        VCU_Pedals.BSE: BSEminVoltageReading,
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
    def standard_mapping(percent_pressed: float) -> float:
        pass
    
    @classmethod
    @abstractmethod
    def inverse_mapping(percent_pressed: float) -> float:
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
    def standard_mapping(percent_pressed: float) -> float:
        return math.sin(percent_pressed * math.pi)


    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return -math.sin(percent_pressed * math.pi) + 1

@AnalogWave.register("SF")
class FullSinusodialWave(AnalogWave):
    """
    Full Sinusodiaul Wave
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return (-(math.cos(percent_pressed *2 * math.pi)) + 1)/2


    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return -math.sin(percent_pressed * math.pi) + 1 


@AnalogWave.register("T")    
class TriangularWave(AnalogWave):
    """
    Linear mapping
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return percent_pressed
    
    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return 1-percent_pressed
    
@AnalogWave.register("R")    
class RandomWave(AnalogWave):
    """
    Random wav
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return random.uniform(0, percent_pressed)
    
    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return random.uniform(0, 1- percent_pressed)
    
@AnalogWave.register("M")    
class MaxWave(AnalogWave):
    """
    Maps to the maximum of pedals voltage, used for stress testing
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return 1
    
    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return 0
    
@AnalogWave.register("O")    
class MinWave(AnalogWave):
    """
    Maps to the minimum of a pedal's voltage, used for excluding a pedal's role in the simulation
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return 0
    
    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return 1

@AnalogWave.register("P") 
class SpikeWave(AnalogWave):
    """
    Used to measure circuit fauls for spikes in pedal voltage readings
    """

    @classmethod
    def standard_mapping(percent_pressed: float) -> float:
        return 1 if percent_pressed == 0 else 0
    
    @classmethod
    def inverse_mapping(percent_pressed: float) -> float:
        return 1 if percent_pressed != 0 else 0
    
class Simulation:

    wave_forms = AnalogWave._registered_waves

    class Arguments(Enum):
        Cycles = "Cycles"
        Precision = "Precision",
        APPS_WAVEFORM = "APPS",
        BSE_WAVEFORM = "BSE"

    class States(Enum):
        ARGS = 1,
        EXIT = 2
        ERROR = 3
    
    CSV_FILE_NAME = "SimulatedValues.csv"

    def __init__(self):
        self.plotted_points: dict[VCU_Pedals, list[float]] = {key: [] for key in VCU_Pedals}
        self.sim_lenght : int = 0
    
    def begin(self):
        self.get_command()

    def get_command(self):
        """
        Wait for the user to 
        """
        while True:
            args = input(">>>")
            ret: Union[bool, dict, None] = self._parse_args(args)

            if type(ret) == dict:
                self.add_simulation(ret)
            if type(ret) == bool:
                if not ret:
                    exit()
                if ret:
                    #succesfully wrote values
                    #TODO: Clear VCU plots
                    pass

        
    def _parse_args(self, args: str):
        """
        Handle the users input from the prompt for the new commands
        """
        exit_keys = [":q", "quit", "exit", "exit()"]
        help_keys = ["-h", "--help", "help"]
        execute_keys = ["-e", "execute", "--execute"]

        match (len(args.split())):
            case 1:
                if args in exit_keys:
                    return exit()
                if args in help_keys:
                    self._generate_help_message()
                    return False
                if args in execute_keys:
                    #Begin writting to VCU
                    return True
                    
            case 4:
                # Regex pattern for matching arguments:
                pattern = r'^(\d+|0|[1-9]\d*) (\d+|0|[3-9]\d*) ([a-zA-Z_]+) ([a-zA-Z_]+)$'
                match = re.match(pattern, args)
                if match:
                    #Generate dictionnary 
                    arg_tuple = match.groups()
                    import pdb; pdb.set_trace()
                    enum_keys_list = [member for member in list(self.Arguments)]
                    args_dict = {key: value for key, value in zip(enum_keys_list, arg_tuple)}
                    #Verify dictionnary
                    if self._verify_args(args=args_dict):
                       print(f"Arguments accepted")
                       return args_dict

                else:
                    print(f"Not matched: {args}")
            case _:
                    print("Invalid input")

    def _verify_args(self, args: dict[Arguments,str]):

        Pass = True
        if args[self.Arguments.APPS_WAVEFORM] not in self.wave_forms:
            print("Invalaid APPS WaveForm")
            Pass = False
        if args[self.Arguments.APPS_WAVEFORM] not in self.wave_forms:
            print("Invalid BSE WaveForm")
            Pass = False

        if args[self.Arguments.APPS_WAVEFORM] == "I" and args[self.Arguments.BSE_WAVEFORM] == "I":
            print("Both APPS and BSE cannot be mapped as inverses")
            Pass = False
        
        #cast to an int
        args[self.Arguments.Cycles] = int(args[self.Arguments.Cycles])
        if args[self.Arguments.Cycles] < 1: 
            print("Cycles must be atleast 1")
            Pass = False

        #cast to an int
        args[self.Arguments.Precision] = int(args[self.Arguments.Precision])
        if args[self.Arguments.Precision] < 1:
            print("Precision must be atleast 3")
            Pass = False

        return Pass
    
    def _generate_help_message(self):
        help_str = ""
        help_str += ("usage: Cycles Precision APPS_Wave BSE_WAVE\n")
        help_str += ("Cycles > 0\n")
        help_str += ("Precision > 3\n") 
        help_str += ("Avail WaveForms:\n")          
        for key,value in self.wave_forms.items():
                help_str += key + ":"
                help_str += value.__doc__.strip('\n') #strip both sides
                help_str += "\n"
        help_str += "I : Inverse of the other pedal argument"
                
    def add_simulation(self, args: dict[Arguments, str]):
        """
        Do the necessary mapping here
        """
        DEFAULT_CYCLES = 1
        DEFAULT_PRECISION = 3
        
        #default to one cycle if key does cycle argument not given
        for cycle in range(1,args.get(self.Arguments.Cycles, DEFAULT_CYCLES)+1):
            print(f"beggining cycle: {cycle}")

            #default to wave precision of 3 if precision argument does not exist                
            for increment in range(args.Pre + 1):
                # NOTE: Percentage always represented as a number [0, 1] not [0, 100]
                curpercentage = increment / args.Precision



   
        for key in VCU_Values.keys():
            VCU_plot_values[key].append(float(VCU_Values[key]))
        
    
    def display_plot(self):
        """
        Display the plot after each additional wave created
        to show that the user has created
        """
        PointPlotting.generate_VCU_plot(VCU_plot_values)

Simulation().begin()

    



    
    
        
        

