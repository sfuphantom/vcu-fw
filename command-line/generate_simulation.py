import re
from wave_forms import AnalogWave, VCU_Pedals, Union, VCU_Pedal
from data_generation import DataGeneration
from enum import Enum
import matplotlib.pyplot as plt
import pandas as pd

class Simulation:

    CSV_FILE_NAME = "SimulatedValues.csv"

    wave_forms = AnalogWave._registered_waves

    def __init__(self):
        self.plotted_points: dict[VCU_Pedals, list[float]] = {key: [] for key in VCU_Pedals}
        self.sim_length: int = 0

    def begin(self):
        """
        Initial point to begin simulation
        """
        return self.get_command()

    def get_command(self):
        """
        Wait for user input
        """
        while True:
            args = input(">>>")
            ret = self._parse_args(args)

            if isinstance(ret, dict):
                self.add_simulation(ret)
            if isinstance(ret, bool):
                if not ret:
                    print(self._generate_help_message())
                if ret:
                    # Successfully wrote values
                    # TODO: Clear VCU plots
                    return True

    def _parse_args(self, args: str):
        """
        Handle user input from the prompt for new commands.
        """
        exit_keys = [":q", "quit", "exit", "exit()"]
        help_keys = ["-h", "--help", "help"]
        execute_keys = ["-e", "execute", "--execute"]
        manual_control = ["MC", "mc", "manual"]

        num_args = len(args.split())

        if num_args == 1:
            if args in exit_keys:
                return exit()
            if args in help_keys:
                self._generate_help_message()
                return False
            if args in execute_keys:
                # Begin writing to VCU
                return True
            if args in manual_control:
                return 1

        if num_args == 4:
            if self._validate_arguments(args):
                return self._parse_arguments(args)
            else:
                print(f"Invalid arguments: {args}")
                return False


        print("Invalid command")

    def _validate_arguments(self, args: str) -> bool:
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
                print(f"Both Waveforms can't be inverses")
                return False
            #wave is found
            return True
        #does not match pattern +int +int str str 
        return False

    def _parse_arguments(self, args: str) -> dict:
        arg_tuple = args.split()
        return {
            'Cycles': int(arg_tuple[0]),
            'Precision': int(arg_tuple[1]),
            'APPS_WAVEFORM': arg_tuple[2],
            'BSE_WAVEFORM': arg_tuple[3]
        }
    

    def _generate_help_message(self) -> str:
        help_str = "usage: Cycles Precision APPS_Wave BSE_WAVE\n"
        help_str += "\n".join([f"{key}: {value.__doc__.strip()}" for key, value in AnalogWave._registered_waves.items()])
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

        self._generate_plot(show = True)
    
    def _format_values(self, data: dict, format_type : type = int):
        #Format data to a specifc format type written as a string
        for key, value in data.items():
            formatted_value = f"{format_type(value)}" 
            data[key] = formatted_value

    def _generate_plot(self, show : bool = False):
        """
        Display the plot after each additional wave created
        to show that the user has created
        """
        print ("\nClose Plot to Continue")
        DataGeneration.generate_VCU_plot(self.plotted_points, showplot=show) 
    
    def _get_percentage(self, wave : str, inverse_wave : str, percent: float) -> float:
        #handle when the wave being called is an inverse
        if wave == "I":
            sim_wave : AnalogWave = self.wave_forms[inverse_wave]
            return sim_wave.inverse_mapping(percent)
        else:
            sim_wave : AnalogWave = self.wave_forms[wave]
            return sim_wave.standard_mapping(percent)
        






    



    
    
        
        

