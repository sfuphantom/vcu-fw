

# Import the argparse library
import time
import serial
import argparse

import os
import sys

#-------------------------Note -----------------------------------:
#This program does not check if the input values are compatible with throttle
#Usually Throttle values are betweeen 0-1, and checking for range faults
#is done through built-in functions (check task_throttle).
#------------------------     --------------------------------------

# Create the parser
my_parser = argparse.ArgumentParser(description='Throttle Value Simulator')


# Add the arguments
my_parser.add_argument('BSE_val',
                       metavar='BSE',
                       type=float,
                       help='Break Sensor : float')

my_parser.add_argument('APPS1_val',
                       metavar='APPS1',
                       type=float,
                       help='Accelerator Pedal 1 : float  ')
                       

my_parser.add_argument('APPS2_val',
                       metavar='APPS2',
                       type=float,
                       help='Accelerator Pedal 2 : float ')                      

# Execute the parse_args() method
#Stores arguments in a Namespacc
args = my_parser.parse_args()


ser = serial.Serial('COM7',9600)
count = 0


print("HELLO?")
mylist = []
mylist.append(args.BSE_val)
mylist.append(args.APPS1_val)
mylist.append(args.APPS2_val)

print(mylist)

'''for i in range(0,3):
    mylist[i] = str(mylist[i])
    ser.write(mylist[i]).encode()
    data = ser.readline()
    print(data)
'''

