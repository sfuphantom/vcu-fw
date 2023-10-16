from wave_forms import AnalogWave, VCU_Pedals, Union, VCU_Pedal
from data_generation import DataGeneration
from enum import Enum
import re

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
        return self.get_command()

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
                    print(self._generate_help_message())
                if ret:
                    #succesfully wrote values
                    #TODO: Clear VCU plots
                    return True

        
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
                    #Generate argument as dictionnary 
                    arg_tuple = match.groups()
                    enum_keys_list = [member for member in list(self.Arguments)]
                    args_dict = {key: value for key, value in zip(enum_keys_list, arg_tuple)}

                    #Verify dictionnary
                    if self._verify_args(args=args_dict):
                       print(f"Arguments accepted")
                       return args_dict
                    else:
                       return False

                else:
                    print(f"Not matched: {args}")
            case _:
                    print("Invalid commands")

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
    
    def _generate_help_message(self) -> str:
        help_str = ""
        help_str += ("usage: Cycles Precision APPS_Wave BSE_WAVE\n")        
        for key,value in self.wave_forms.items():
                help_str += key + ":"
                help_str += value.__doc__.strip('\n') #strip both sides
                help_str += "\n"
        return help_str
    
    def format_values(self, data: dict, format_type : type = int):
        #Format data to a specifc format type written as a string
        for key, value in data.items():
            formatted_value = f"{format_type(value)}" 
            data[key] = formatted_value
                
    def add_simulation(self, args: dict[Arguments, str]):
        """
        Do the necessary mapping here
        """
        DEFAULT_CYCLES = 1
        DEFAULT_PRECISION = 3

        apps_wave: str = args[self.Arguments.APPS_WAVEFORM]
        bse_wave: str = args[self.Arguments.BSE_WAVEFORM]

        vcu_values: dict[VCU_Pedals, float] = {key: 0 for key in VCU_Pedals}
        
        #default to one cycle if key does cycle argument not given
        for cycle in range(1,args.get(self.Arguments.Cycles, DEFAULT_CYCLES)+1):
            print(f"beggining cycle: {cycle}")

            precision = args.get(self.Arguments.Precision, DEFAULT_PRECISION) + 1
            #default to wave precision of 3 if precision argument does not exist                
            for increment in range(precision):
                # NOTE: Percentage always represented as a number [0, 1] not [0, 100]
                current_percentage = increment / precision

                #Handle APPS
                sim_wave : AnalogWave = self.wave_forms[apps_wave]
                mapped_percent = sim_wave.standard_mapping(current_percentage)

                if apps_wave == "I":
                    sim_wave : AnalogWave = self.wave_forms[bse_wave]
                    mapped_percent = sim_wave.inverse_mapping(current_percentage)


                sim_wave.set_values(VCU_Pedal(VCU_Pedals.APPS1), mapped_percent, vcu_values)
                sim_wave.set_values(VCU_Pedal(VCU_Pedals.APPS2), mapped_percent, vcu_values)

                #HANDLE BSE
                sim_wave : AnalogWave = self.wave_forms[bse_wave]
                mapped_percent = sim_wave.standard_mapping(current_percentage)
                if bse_wave == "I":
                    sim_wave : AnalogWave = self.wave_forms[apps_wave]
                    mapped_percent = sim_wave.inverse_mapping(current_percentage)


                sim_wave.set_values(VCU_Pedal(VCU_Pedals.BSE), mapped_percent, vcu_values)

                self.format_values(vcu_values)

                    
                #write values to plot
                for key in vcu_values.keys():
                    self.plotted_points[key].append(float(vcu_values[key]))

        self.display_plot()
        
    
    def display_plot(self):
        """
        Display the plot after each additional wave created
        to show that the user has created
        """
        print ("\nClose Plot to Continue")
        DataGeneration.generate_VCU_plot(self.plotted_points)



    



    
    
        
        

