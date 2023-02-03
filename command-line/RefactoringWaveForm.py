#Author: kevinl03 

import random;
import argparse 
import math
import Throttle_Value_Simulator
import time

import csv



#---------------------------------------------------------------------------------#
#THESE VARIABLES CAN BE CHANGED DEPENDING ON PEDAL/VCU SPECIFICATIONS
#Most up to date specifications found here :
# https://docs.google.com/document/d/1PsZSJR6la_u_oXmN2lo8tfRcU6E8GrMdpH35ESl7d4U/

#ranges for VCU voltage
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
def mapPercentageToAPPS1Voltage(percentPressed):
    return percentPressed*(APPS1maxVoltageReading - APPS1minVoltageReading) + APPS1minVoltageReading

def mapPercentageToAPPS2Voltage(percentPressed):
    return percentPressed*(APPS2maxVoltageReading - APPS2minVoltageReading) + APPS2minVoltageReading

def mapPercentageToBSEVoltage(percentPressed):
    return percentPressed*(BSEmaxVoltageReading - BSEminVoltageReading)+ BSEminVoltageReading


#Sin wave used for to simulate a car going around a turn where
#The applied press is different when exiting a turn 
def percentToSinWave(percent):

    HorizontalScale = math.pi

    return (math.sin(percent*HorizontalScale))

#Takes the value of percent pressed and reflected over the line y = 0.5
#Such that the percent is 1-(sinwave(percentpressed)

def percentToInverseSinWave(percent):

    HorizontalScale = math.pi

    return (-math.sin(percent*HorizontalScale)+1)




#Generates the values for the Sinwave
def sinWave(PedalType,PercentPressed, Values):
    


    SinPercentage = percentToSinWave(PercentPressed)
    match PedalType:
        case "APPS1":
            Values[0] = mapPercentageToAPPS1Voltage(SinPercentage)
            
        case "APPS2":
            Values[1] = mapPercentageToAPPS2Voltage(SinPercentage)

        case "BSE":
            Values[2] = mapPercentageToBSEVoltage(SinPercentage)

def InversesinWave(PedalType,PercentPressed, Values):

    SinPercentage = percentToInverseSinWave(PercentPressed)
    Values[2] = mapPercentageToBSEVoltage(SinPercentage)


#Simulated Cons
def triangularWave(PedalType,PercentPressed, Values):
    
    match PedalType:
        case "APPS1":
            Values[0] = mapPercentageToAPPS1Voltage(PercentPressed)
            
        case "APPS2":
            Values[1] = mapPercentageToAPPS2Voltage(PercentPressed)

        case "BSE":
            Values[2] = mapPercentageToBSEVoltage(PercentPressed)



#Generate random values to test firmware when voltage readings are discontinuous

def scatteredWave(PedalType,PercentPressed,Values):
    randomPercentPressed = random.randint(0,int(PercentPressed*100))
    randomPercentPressed = float(randomPercentPressed/100)
    match PedalType:
        case "APPS1":
            Values[0] = mapPercentageToAPPS1Voltage(randomPercentPressed)
            
        case "APPS2":
            Values[1] = mapPercentageToAPPS2Voltage(randomPercentPressed)

        case "BSE":
            Values[2] = mapPercentageToBSEVoltage(randomPercentPressed)


#Create 1 spike per wave form. Precision will control how
#values seperate each spike, and cycles will determine the total
#amount of spikes per simulation
def spikeWave(PedalType,PercentPressed,Values):

    
    if (PercentPressed == 0):
    
        match PedalType:
            case "APPS1":
                Values[0] = APPS1maxVoltageReading
                
            case "APPS2":
                Values[1] = APPS2maxVoltageReading

            case "BSE":
                Values[2] = BSEmaxVoltageReading
    else:
        match PedalType:
            case "APPS1":
                Values[0] = APPS1minVoltageReading
                
            case "APPS2":
                Values[1] = APPS2minVoltageReading

            case "BSE":
                Values[2] = BSEminVoltageReading
        



#Used for choosing which wave to generate     
def matchInverse(APPSWaveForm,curpercentage,Values):
    match APPSWaveForm:

        case "S":
            InversesinWave("BSE",curpercentage, Values)
                        
        case "T":
             #Passing 1-curpercentage will give the percentage as a float which is the opposite of
             #the other reading
            triangularWave("BSE",1-curpercentage, Values)                        
        case "R":
            scatteredWave("BSE", 1-curpercentage, Values)
        
        case "P":
            
            spikeWave("BSE",1-curpercentage, Values)
                            
        case "M":
            Values[2] = BSEminVoltageReading
        
        case "O":
            Values[2] = BSEmaxVoltageReading
        case _:
            pass

#check that the input arguments are legal inputs
def checkArguments(cycles, precision, APPS, BSE):
    APPSWaves = ["S", "T", "R", "P", "M", "O"]
    BSEWaves =  ["S", "T", "R", "P", "M", "O", "I"]
    errorfound = False
    
    if (cycles < 1):
        print("cycles must be at least 1")
        errorfound = True
    if (precision < 3):
        print("precision must be atleast 3")
        errorfound = True
    if (APPS not in APPSWaves):
        print(APPS, "is not an available waveform to model the APPS")
        errorfound = True
    if (BSE not in BSEWaves):
        print(BSE, "is not an available waveform to model the BSE")
        errorfound = True
    if (errorfound):
        exit()
    else:
        pass
        #TODO: Total Time is a theoretical estimate of how long a simulation shold take
        #TotalTime= cycles*precision*(Throttle_Value_Simulator.delayVCUReceiveValues())
        #Actual running time takes 50% longer
        #print("Simulation will take", "%.3f" % TotalTime, "seconds to complete")
        

        
    
    
    

if __name__ == "__main__":


    my_parser = argparse.ArgumentParser(description='Arbitrary Wave Form Generator')

    
    #Create arguments when calling the script
    
    my_parser.add_argument('Cycles',
                           metavar='Cyles',
                           type=int,
                           help=' Range : int >= 1')

    my_parser.add_argument('Precision',
                           metavar='Precision',
                           type=int,
                           help='''The amount of values to represent each waveform:
                           Higher precision results in a more accurate simulation for the waveform''')

    my_parser.add_argument('APPSWaveForm',
                           metavar='APPSWaveForm',
                           type=str,
                           help='T = triangular S = sinusodial \n R = Random \n P = Spike \n     M = Max voltage \n O = Min voltage ')

    my_parser.add_argument('BSEWaveForm',
                           metavar='BSEWaveForm',
                           type=str,
                           help='T = triangular \n S = sinusodial \n R = Random \n P = Spike \n  M = Max voltage \n O = Min voltage I = Inverse of APPS')


    #read values from the commandline/terminal
    args = my_parser.parse_args()

    

    

    #pass newline ='' as argument to avoid spaces between excel rows    
    with open('csv_file.csv', 'w', newline = '') as f:
        
        numcycles = args.Cycles
        precision = args.Precision

        BSEWaveForm= args.BSEWaveForm
        APPSWaveForm = args.APPSWaveForm

        checkArguments(numcycles,precision,APPSWaveForm,BSEWaveForm)

        csv.writer(f).writerow(["APPS1","APPS2","BSE"])
        

        #placeholder for values [APPS1, APPS2, BSE]
        #Will be changed based on the arguments and the current percentage pressed

        Values = [0,0,0]

        for cycle in range(numcycles):


            
            #Total number of values to represent the chosen wave
            
            for increment in range(0,precision+1):
                #NOTE: percentage always represented as a number [0,1] not [0,100]
                curpercentage = (increment/(precision))
                
                #Sets values [APPS1, APPS2 ,    ]
                match APPSWaveForm:
                    case "S":
                        sinWave("APPS1",curpercentage, Values)
                        sinWave("APPS2",curpercentage, Values)
                        
                    case "T":
                        triangularWave("APPS1",curpercentage, Values)
                        triangularWave("APPS2",curpercentage, Values)
                        
                    case "R":
                        scatteredWave("APPS1",curpercentage, Values)
                        scatteredWave("APPS2",curpercentage, Values)

                    case "P":
                        spikeWave("APPS1",curpercentage, Values)
                        spikeWave("APPS2",curpercentage, Values)

                    case "M":
                        Values[0] = APPS1maxVoltageReading 
                        Values[1] = APPS2maxVoltageReading

                    case "O":
                        #lowest possible values
                        Values[0] = APPS1minVoltageReading 
                        Values[1] = APPS2minVoltageReading

                    case _:
                        #will never get to this point
                        pass
                        
                #Sets values  [ , , BSE] 
                match BSEWaveForm:
                    case "S":
                        sinWave("BSE",curpercentage, Values)
                        
                    case "T":
                        triangularWave("BSE",curpercentage, Values)
                        

                    case "R":
                        scatteredWave("BSE",curpercentage, Values)

                    case "I":     
                        matchInverse(APPSWaveForm,curpercentage,Values)

                    case "P":
                        spikeWave("BSE",curpercentage, Values)

                    case "M":
                        Values[2] = BSEmaxVoltageReading
                            
                    case "O":
                        Values[2] = BSEminVoltageReading
                    case _:
                        #will never get to this point
                        pass
                           
                #Writes the set of values to the CSV file           
                csv.writer(f).writerow(Values)           
                

            

    #Defined in ThrottleValue Simulator 
    Throttle_Value_Simulator.sendValsFromFile('csv_file.csv') #uncomment this line to send values to VCU
