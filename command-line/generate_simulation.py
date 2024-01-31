#Author : Kevin Litvin
#Date : November 2023
#Description : Defines the interface for generating simulation models from command line inputs

import re
from wave_forms import AnalogWave, VCU_Pedals, Union, VCU_Pedal
from data_generation import DataGeneration
from enum import Enum
import matplotlib.pyplot as plt
import pandas as pd
from StatesAndEvents import ResponseVCU
from vcu_communication import VCU_Communication
from vcu_communication import (
	APPS1_MIN,
	APPS1_MAX,
	APPS2_MIN,
	APPS2_MAX,
	BSE_MIN,
	BSE_MAX
)

class Simulation:

    CSV_FILE_NAME = "SimulatedValues.csv"

    wave_forms = AnalogWave._registered_waves

    def __init__(self):
        self.plotted_points: dict[VCU_Pedals, list[float]] = {key: [] for key in VCU_Pedals}
        self.sim_duration: int = 0

    def _parse_args(self, args: str) -> Union[dict, tuple, bool]:
        """
        Handle user input from the prompt for new commands.
        """
        exit_keys = [":q", "quit", "exit", "exit()"]
        help_keys = ["-h", "--help", "help"]
        execute_keys = ["-e", "execute", "--execute"]

        num_args = len(args.split())

        if num_args == 1:
            if args in exit_keys:
                return exit()
            if args in help_keys:
                print(self._generate_help_message())
                return False
            if args in execute_keys:
                # Begin writing to VCU
                return True

        if num_args == 4:
            if self._validate_sim_arguments(args):
                return self._build_arguments(args)
            else:
                print(f"Invalid Sim Arguments: {args}")
                return False
   
        if num_args == 7:
            if self.validate_manual_control_arguments(args):
                return self.parse_manual_mode_arguments(args)
            else:
                print(f"Invalid Manual Control Arguments: {args}")
                return False
        
        print(f"Uknown args - {args}")
        
        

    def _validate_sim_arguments(self, args: str) -> bool:
        """
        Verify the arguments for the simulation
        """
        pattern = r'^(\d+|0|[1-9]\d*) (\d+|0|[3-9]\d*) ([a-zA-Z_]+) ([a-zA-Z_]+)$'
        match = re.match(pattern, args)
        if match:
            arg_tuple = args.split()
            #check for valid waves to avoid key errors
            if arg_tuple[2] not in self.wave_forms:
                print(f"Unknown Wave {arg_tuple[2]}")
                return False
            if arg_tuple[3] not in self.wave_forms:
                print(f"Uknown Wave {arg_tuple[3]}")
                return False
            if arg_tuple[2] == "I" and arg_tuple[3] == "I":
                print(f"Both Waveforms can not be inverses")
                return False
            #wave is found
            return True
        #does not match pattern +int +int str str 
        return False

    def _build_arguments(self, args: str) -> dict:
        arg_tuple = args.split()
        return {
            'Cycles': int(arg_tuple[0]),
            'Precision': int(arg_tuple[1]),
            'APPS_WAVEFORM': arg_tuple[2],
            'BSE_WAVEFORM': arg_tuple[3]
        }
    
    def validate_manual_control_arguments(self, args: str) -> bool:
        """
        Verify the arguments for manual control mode
        """
        pattern = r'^MC (\d{1,}) (\d{1,}) (\d{1,}) (True|False) (True|False) (True|False)$'
        if bool(re.match(pattern, args)):
            arg_tuple = args.split()
            if int(arg_tuple[1]) not in range(APPS1_MIN, APPS1_MAX+1):
                print(f"Invalid Range APPS1 - {arg_tuple[1]}")
                return False
            if int(arg_tuple[2]) not in range(APPS2_MIN, APPS2_MAX+1):
                print(f"Invalid Range APPS2 - {arg_tuple[2]}")
                return False
            if int(arg_tuple[3]) not in range(BSE_MIN, BSE_MAX+1):
                print(f"Invalid Range BSE - {arg_tuple[3]}")
                return False
            return True
        return False

    def parse_manual_mode_arguments(self, args: str) -> tuple:
        arg_tuple = args.split()
        int_args = [int(val) for val in arg_tuple[1:4]]
        #check for true or false in args, without them needing to type the full True or False
        bool_args = [val == 'True' for val in arg_tuple[4:7]]
        return tuple(int_args + bool_args)
    
    def _generate_help_message(self) -> str:
        help_str = "Sim Model Usage: Cycles Precision APPS_Wave BSE_WAVE\n\n"
        help_str += "------------REGISTERED WAVES------------\n"
        help_str += "\n".join([f"{key}: {value.__doc__.strip()}" for key, value in AnalogWave._registered_waves.items()])
        help_str += "\n---------------------------------------"
        help_str +="\n\nManual Control Usage : MC APPS1 APPS2 BSE TSAL RTD SETRESET\n"
        return help_str

    def add_simulation(self, args: dict):

        DEFAULT_CYCLES = 1
        DEFAULT_PRECISION = 3

        apps_wave = args['APPS_WAVEFORM']
        bse_wave = args['BSE_WAVEFORM']
       
        sim_wave = AnalogWave
        vcu_values = {key: 0 for key in VCU_Pedals}

        for cycle in range(1, args.get('Cycles', DEFAULT_CYCLES) + 1):
            print(f"beginning cycle: {cycle}")
            precision = args.get('Precision', DEFAULT_PRECISION) + 1

            for increment in range(precision):
                 # NOTE: Percentage always represented as a number [0, 1] not [0, 100]
                current_percentage = increment / precision
                
                #Handle APPS
                apps_mapped_percent = self._get_percentage(apps_wave, bse_wave, current_percentage)

                sim_wave.set_values(VCU_Pedal(VCU_Pedals.APPS1), apps_mapped_percent, vcu_values)
                sim_wave.set_values(VCU_Pedal(VCU_Pedals.APPS2), apps_mapped_percent, vcu_values)

                #HANDLE BSE
                bse_mapped_percent = self._get_percentage(bse_wave, apps_wave, current_percentage)
                sim_wave.set_values(VCU_Pedal(VCU_Pedals.BSE), bse_mapped_percent, vcu_values)
                
                self._format_values(vcu_values)
                #write values to plot
                for key in vcu_values.keys():
                    self.plotted_points[key].append(float(vcu_values[key]))

                self.sim_duration += VCU_Communication.LATENCY
    
    def _format_values(self, data: dict, format_type : type = int):
        #Format data to a specifc format type written as a string
        for key, value in data.items():
            formatted_value = f"{format_type(value)}" 
            data[key] = formatted_value

    
    def _get_percentage(self, wave : str, inverse_wave : str, percent: float) -> float:
        #handle when the wave being called is an inverse
        if wave == "I":
            sim_wave : AnalogWave = self.wave_forms[inverse_wave]
            return sim_wave.inverse_mapping(percent)
        else:
            sim_wave : AnalogWave = self.wave_forms[wave]
            return sim_wave.standard_mapping(percent)
        






    



    
    
        
        

