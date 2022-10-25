
#Kevinl03

import serial
from serial import Serial
import serial.tools.list_ports
import argparse
import operator

import os
import sys

#-------------------------Note -----------------------------------:
#This program does not check if the input values are compatible with throttle
#Usually Throttle values are betweeen 0-1, and checking for range faults
#is done through built-in functions (check task_throttle).
#------------------------     --------------------------------------


#Following functions
def get_available_serial_ports():
    available_ports_all = list(serial.tools.list_ports.comports())               # get all available serial ports
    available_ports = [port for port in available_ports_all if port[2] != 'n/a'] # remove all unfit serial ports
    available_ports.sort(key=operator.itemgetter(1))                             # sort the list based on the port
    return available_ports

def select_a_serial_port(available_ports):                                       # TODO: check file_cfg for preselected serial port
    global selected_port
    if len(available_ports) == 0:       # list is empty -> exit
        print("[!] No suitable serial port found.")
        exit(-1)
    elif len(available_ports) == 1:     # only one port available
        (selected_port,_,_) = available_ports[0]
        print("[+] Using only available serial port: %s" % selected_port)
    else:                               # let user choose a port
        successful_selection = False
        while not successful_selection:
            print("[+] Select one of the available serial ports:")
            # port selection
            item=1
            for port,desc,_ in available_ports:
                print ("    (%d) %s \"%s\"" % (item,port,desc))
                item=item+1
            selected_item = int(input(">>> "))                               # TODO: handle character input
            # check if a valid item was selected
            if (selected_item > 0) and (selected_item <= len(available_ports)):
                (selected_port,_,_) = available_ports[selected_item-1]
                print(available_ports[selected_item-1])
                successful_selection = True
            else:
                print("[!] Invalid serial port.\n")

if __name__ == "__main__":

    #import serial


    #os.system('cmd /k "pip install serial"')

    
    # Create the parser
    my_parser = argparse.ArgumentParser(description='Throttle Value Simulator')


    # Add the arguments
    my_parser.add_argument('BSE_val',
                           metavar='BSE',
                           type=int,
                           help='Break Sensor :     0 <= int <= 1000')

    my_parser.add_argument('APPS1_val',
                           metavar='APPS1',
                           type=int,
                           help='Accelerator Pedal 1 : 0 <= int <= 1000  ')
                           

    my_parser.add_argument('APPS2_val',
                           metavar='APPS2',
                           type=int,
                           help='Accelerator Pedal 2 : 0 <= int <= 1000 ')                      

    # Execute the parse_args() method
    #Stores arguments in a Namespacc
    args = my_parser.parse_args()

    #get the ports and set the ports
    select_a_serial_port(get_available_serial_ports())


    ser = serial.Serial(selected_port,9600)



    #create a list 
    mylist = []
    mylist.append(args.BSE_val)
    mylist.append(args.APPS1_val)
    mylist.append(args.APPS2_val)



    # Write text or bytes to the file
    for word in mylist:
        ser.write(word)
        ser.write(",".encode('utf-8'))




    


