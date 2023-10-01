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

    # Pass newline='' as an argument to avoid spaces between Excel rows
    with open(CSV_FILE_NAME, 'w', newline='') as f:
        

        #Write the header
        csv.writer(f).writerow([key for key in VCU_Values.keys()])

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