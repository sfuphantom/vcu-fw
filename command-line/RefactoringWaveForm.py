import os;
import random;
import argparse
import math
#import Throttle_Value_Simulator

import csv



#-------------------------------------------------------------------#
#THESE VARIABLES CAN BE CHANGED DEPENDING ON PEDAL/VCU SPECIFICATIONS
#Can be found here : https://docs.google.com/document/d/1PsZSJR6la_u_oXmN2lo8tfRcU6E8GrMdpH35ESl7d4U/edit

#ranges for petals

maxPercentPressed = 50
minPercentPressed = 0

maxVoltageReading = 5
minVoltageReading = 0

#ranges for VCU voltage
APPS1maxVoltageReading = 4.5
APPS1minVoltageReading = 1.50

APPS2maxVoltageReading = 1.49
APPS2minVoltageReading = 0


BSEmaxVoltageReading = 4.5
BSEminVoltageReading = 1.50

APPSMaxPedalAngle = 225
APPSMinPedalAngle = 180

BSEMaxPedalAngle = 135
BSEMinPedalAngle = -74



#-------------------------------------------------------------------#




def mapPercentageToAPPS1Voltage(percentPressed):
    return percentPressed*(APPS1maxVoltageReading - APPS1minVoltageReading) + APPS1minVoltageReading

def mapPercentageToAPPS2Voltage(percentPressed):
    return percentPressed*(APPS2maxVoltageReading - APPS2minVoltageReading) + APPS2minVoltageReading

def mapPercentageToBSEVoltage(percentPressed):
    return percentPressed*(BSEmaxVoltageReading - BSEminVoltageReading)+ BSEminVoltageReading


def percentToSinWave(percent):
    
    VerticalScale = percent

    HorizontalScale = math.pi

    return (math.sin(percent*HorizontalScale))

def percentToInverseSinWave(percent):

    HorizontalScale = math.pi

    return (-math.sin(percent*HorizontalScale)+1)


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


def triangularWave(PedalType,PercentPressed, Values):
    
    match PedalType:
        case "APPS1":
            Values[0] = mapPercentageToAPPS1Voltage(PercentPressed)
            
        case "APPS2":
            Values[1] = mapPercentageToAPPS2Voltage(PercentPressed)

        case "BSE":
            Values[2] = mapPercentageToBSEVoltage(PercentPressed)


def scatteredWave(PedalType,PercentPressed, Values):

    randomPercentPressed = random.int(0,int(PercentPressed*100))
    match PedalType:
        case "APPS1":
            Values[0] = mapPercentageToAPPS1Voltage(randomPercentPressed)
            
        case "APPS2":
            Values[1] = mapPercentageToAPPS2Voltage(randomPercentPressed)

        case "BSE":
            Values[2] = mapPercentageToBSEVoltage(randomPercentPressed)
        



    
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
                            pass
                            

                        case "O":
                           Values[2] = BSEmaxVoltageReading
                        case _:
                            pass
    

if __name__ == "__main__":

    my_parser = argparse.ArgumentParser(description='Arbitrary Wave Form Generator')

    my_parser.add_argument('Cycles',
                           metavar='Cyles',
                           type=int,
                           help=' Range :     0 <= int <= 100')

    my_parser.add_argument('Precision',
                           metavar='Precision',
                           type=int,
                           help='''The amount of values to represent each waveform:
                           Higher precision results in a more accurate simulation for the waveform''')

    my_parser.add_argument('APPSWaveForm',
                           metavar='APPSWaveForm',
                           type=str,
                           help='T = triangular \n S = sinusodial \n R = Random \n P = Spike Values \n     O = APPS Values set to 0 ')

    my_parser.add_argument('BSEWaveForm',
                           metavar='BSEWaveForm',
                           type=str,
                           help='T = triangular \n S = sinusodial \n R = Random \n I = Inverse of APPS \n O = Brake Values set to 0 ')


    args = my_parser.parse_args()

    

    #pass newline ='' as argument to avoid spaces between excel rows    
    with open('csv_file.csv', 'w', newline = '') as f:
        
        numcycles = args.Cycles
        precision = args.Precision

        BSEWaveForm= args.BSEWaveForm
        APPSWaveForm = args.APPSWaveForm

        #placeholder for set of values which will be filled in 

        Values = [0,0,0]

        for cycle in range(numcycles):


            for increment in range(0,precision+1):
                curpercentage = (increment/precision)
                
            
                 
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
                        pass
                        

                    case "O":
                        #lowest possible values
                        Values[0] = APPS1minVoltageReading 
                        Values[1] = APPS2minVoltageReading

                    case _:
                        print(APPSWaveForm, "Not an option")
                        

                match BSEWaveForm:
                    case "S":
                        sinWave("BSE",curpercentage, Values)
                        
                    case "T":
                        triangularWave("BSE",curpercentage, Values)
                        

                    case "R":
                        scatteredWave("BSE",curpercentage, Values)

                    case "I":     
                        matchInverse("BSE",curpercentage,Values)
                            
                    case "O":
                        Values[2] = BSEminVoltageReading
                    case _:
                        pass
                           
                           
                        
                csv.writer(f).writerow(Values)           
                

            
        
        #APPS wave forms must allow to be tested independantly from BSE

    #Defined in ThrottleValue Simulator 
    #Throttle_Value_Simulator.sendValsFromFile('csv_file.csv') #uncomment this line to send values to VCU
