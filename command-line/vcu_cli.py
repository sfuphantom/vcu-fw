from collections import defaultdict
import json
from ntpath import join
import time
from serial import (
    Serial,
    EIGHTBITS,
    STOPBITS_TWO
)
from argparse import ArgumentParser
from enum import Enum, auto

class FirmwareCommand(Enum):
    TASK_NUM = 1 # uxTaskGetNumberOfTasks
    TASK_LIST = auto() # vTaskList
    TASK_SUSPEND = auto() 
    TASK_RESUME = auto()
    ECHO_THROTTLE = auto() # 
    STAT_RUN = auto() # vTaskGetRunTimeStats
    STAT_START = auto() # xTaskGetTickCount
    ECHO_APPS = auto()



class ANSI_COLORS:

    WHITE = '\033[39m'
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'

COLOR_MAP = {
    'R' : ANSI_COLORS.YELLOW,
    'B' : ANSI_COLORS.RED
}

def extract_fw(fwcmd:Enum, index):
    return fwcmd.name.lower().split('_')[index]

def color_palette(row:str):
    c_msg = next(color for state, color in COLOR_MAP.items() if state in row)
    c_msg += row
    c_msg += ANSI_COLORS.WHITE
    return c_msg

def compact(lst):
    return list(filter(None, lst))


class CommandBoard:

    __cmd_map:dict
    ser:Serial
    CMD_MAP:dict

    def __init__(self, port:str, sensor_list:list) -> None: 
        self.ser = Serial(port=port, baudrate=9600, bytesize=EIGHTBITS, stopbits = STOPBITS_TWO, timeout=5)

        self.__cmd_map = defaultdict(list)
        for fwcmd in FirmwareCommand:
            top, sub = fwcmd.name.lower().split('_')
            self.__cmd_map[top].append(sub)

        
    def get_cmd_map(self):
        return self.__cmd_map

    def construct_serial(self, args:dict) -> bytes:
        user_cmd     = next( f'{top_p}_{sub_p}' for top_p, sub_p in args.items() if sub_p)
        firmware_cmd = next( fwcmd for fwcmd in FirmwareCommand if fwcmd.name == user_cmd.upper() )
        serial_msg   = str(firmware_cmd.value).encode('utf-8')
        return firmware_cmd, serial_msg

    def receive_serial(self):
        try:

            msg_buffer = self.ser.read_all().decode()
            return msg_buffer
        except Exception as e:
            print(repr(e))
            return ''

    def send_serial(self, msg):
        self.ser.write(msg)

    def setup_argparse(self, parser:ArgumentParser):
        subparsers = parser.add_subparsers()
        for cmd_type, choices in self.__cmd_map.items(): 
            tmp_parser = subparsers.add_parser(cmd_type)
            tmp_parser.add_argument(cmd_type, choices=choices)


    def run_cli(self):

        while True:
            
            # setup argparse
            cmd_list = input('>> ').split()

            parser = ArgumentParser()
            self.setup_argparse(parser)

            
            # extract command
            try:
                args = vars(parser.parse_args(cmd_list))
            except SystemExit:
                continue

            cmd, msg = self.construct_serial(args)
            
            for _ in range(2): 
                self.send_serial(msg)
                time.sleep(0.5)
            # print(msg)
            # time.sleep(1)
            fw_rx = self.receive_serial()

            if cmd == FirmwareCommand.TASK_LIST:
                ret = ''
                ret += f"{ANSI_COLORS.WHITE}**********************************\n"
                ret += "Task  State   Prio    Stack    Num\n" 
                ret += "**********************************\n"
            
                tasks = compact(fw_rx.split('\n'))
                if tasks:
                    print(tasks)
                    ret += f'{ANSI_COLORS.GREEN}{tasks[0]}{ANSI_COLORS.WHITE}\n'
                    ret += '\n'.join(map(color_palette, tasks[1:]))
                
                ret += f"{ANSI_COLORS.WHITE}\n**********************************\n"
                print(ret)
                # print(cmd)    

            elif cmd == FirmwareCommand.ECHO_APPS:
                sensor_vals = compact( fw_rx.split('\n') )
                if sensor_vals: 
                    print('APPS1: {}% , APPS2: {}% , BSE: {}%'.format(*(s.strip('\r') for s in sensor_vals) ))
            
            else: 
                print(fw_rx)


if __name__ == '__main__':

    board = CommandBoard('COM13', ['throttle'])
    print(json.dumps(board.get_cmd_map(), indent=4))
    board.run_cli()