import os;
import random;
import argparse
import math
import Throttle_Value_Simulator

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
APPS2minVoltageReading = 0.0


BSEmaxVoltageReading = 4.5
BSEminVoltageReading = 1.50

APPSMaxPedalAngle = 225
APPSMinPedalAngle = 180

BSEMaxPedalAngle = 135
BSEMinPedalAngle = -74



#-------------------------------------------------------------------#

def linearPercentToVoltMapping(percent):
    return ((percent/maxPercentPressed)*maxVoltageReading)

####-----------------CREATING NEW FUNCTIONS DURING REFACTOR-------####

#these functions will replace linearPercentToVoltMapping, mapVoltToAPPS1, mapVoltToAPPS2


def mapAngleToPercentPressed(Angle,PedalType):
    match PedalType:

        #Since Accelerator maps to two unique voltages, we use the same calculation
        #For APPS1 as we do for APPS2 
        
        case "APPS1":
            return Angle/(APPSMaxPedalAngle - APPSMinPedalAngle)
        case "APPS2":
            return Angle/(APPSMaxPedalAngle - APPSMinPedalAngle)

    
        
        case "BSE":
            return Angle/(BSEMaxPedalAngle - BSEMinPedalAngle) 
        case _:
            return 0 #default case

def mapPedalAngleToVoltage(Angle, PedalType):
    percentPressed = mapAngleToPercentPressed(Angle,PedalType)
    match PedalType:
        case "APPS1":
            return percentPressed*(APPS1maxVoltageReading - APPS1minVoltageReading)
        case "APPS2":
            return percentPressed*(APPS2maxVoltageReading - APPS2minVoltageReading)
        case "BSE":
            return percentPressed*(BSEmaxVoltageReading - BSEminVoltageReading)
        case _:
            return 0
            


###-----------------------------------------------------------------####

#voltage maps identically to APPPS1
def mapVoltToAPPS1(voltage):
    return voltage

#voltage will map with an offset for APPS2
#values are required to be different for faults

def mapVoltToAPPS2(voltage):
    #To be implemented 
    return voltage

def percentToCosWave(percent):
    
    VerticalScale = -maxPercentPressed/2 #negative so that function increased starting from beggining
    
    HorizontalScale = math.pi/maxPercentPressed #max percentage 50 maps to pi (max value)

    VerticalShift = maxPercentPressed/2 #values cannot be negative
    
    mapping = VerticalScale*(math.cos(percent*HorizontalScale)) + VerticalShift

    return mapping
    

#/|/|/| shape, not /\/\/\/\ like typical triangularWaves
def triangularWave(cycles, APPSForm, BSEForm,f):
    
    writer = csv.writer(f)
    curAPPSAngle = APPSminPedalAngle 
    pedalVals = [0,0,0]
    #25 total increments, and since maxpedal = 50
    #and the VCU accepts data every 20ms, this would model
    #pressing the pedal down all the way in 0.5 seconds

    # 50/2 = 25
    #25 * 20ms = 500ms = 1/2 second
    increment = 2

    #
    for cycle in range(cycles):
        
        while (curAPPSAngle <= APPSmaxPercentPressed):

            match triangularWave

            
            APPS1 =  mapPedalAngleToVoltage(curAPPSAngle, "APPS1")
            APPS2 =  mapPedalAngleToVoltage(curAPPSAngle, "APPS2")
            BSE =    mapPedalAngleToVoltage(curAPPSAngle, "BSE")


            pedalVals[0] = APPS1
            pedalVals[1] = APPS2

            


            #arr[2] = 0 -> currently only testing while break percent is 0
            writer.writerow(array)
            curpercent+= increment

        curpercent = 0


def sineWave(cycles, BSEForm,f):

    writer = csv.writer(f)
    array = [0,0,0]
    increment = 2 #lower this value to make the cosine cycle more accurate

    for cycle in range(cycles):
        curpercent = 0
        while(curpercent < maxPercentPressed):
            wavemapping = percentToCosWave(curpercent)

            pedalAngleToVolt = round(linearPercentToVoltMapping(wavemapping),3)
            
            APPS1 =  mapVoltToAPPS1(pedalAngleToVolt)
            APPS2 =  mapVoltToAPPS2(pedalAngleToVolt)
            
            
            array[0] = APPS1
            array[1] = APPS2


            #arr[2] = 0 -> currently only testing while break percent is 0
            writer.writerow(array)
            
            curpercent+=increment

            
        #comment this while loop to remove decreasing part of the sin wave
        while (curpercent >= 0):
            wavemapping = percentToCosWave(curpercent)

            
            #turn the value to a voltage then round to 3 decimals
            pedalAngleToVolt = round(linearPercentToVoltMapping(wavemapping),3)
            
            APPS1 =  mapVoltToAPPS1(pedalAngleToVolt)
            APPS2 =  mapVoltToAPPS2(pedalAngleToVolt)
            
            
            array[0] = APPS1
            array[1] = APPS2


            #arr[2] = 0 ->break percent will remain 0
            writer.writerow(array)
            
            curpercent -= increment
        

def RandomVals(cycles, BSEForm, f):
    
    writer = csv.writer(f)
    array = [0,0,0]

    for cycle in range(cycles):

        #generate 50 random readings per cycle
        numRandomPercentages = 50
        for totalSamples in range(numRandomPercentages):

            
            randompercent = random.randint(minPercentPressed,maxPercentPressed)
            
            pedalAngleToVolt = round(linearPercentToVoltMapping(randompercent),3)
            
            APPS1 =  mapVoltToAPPS1(pedalAngleToVolt)
            APPS2 =  mapVoltToAPPS2(pedalAngleToVolt)
            
            
            array[0] = APPS1
            array[1] = APPS2


            #arr[2] = 0 ->break percent will remain 0
            writer.writerow(array)
            

            
        
        
            
        
        
 
if __name__ == "__main__":

    my_parser = argparse.ArgumentParser(description='Arbitrary Wave Form Generator')

    my_parser.add_argument('Cycles',
                           metavar='Cyles',
                           type=int,
                           help=' Range :     0 <= int <= 100')

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

        BSEtype = args.BSEWaveForm
        APPStype = args.APPSWaveForm 
        
        if (args.APPSWaveForm == "T"):
            triangularWave(numcycles,BSEtype,f)
        if (args.APPSWaveForm == "S"):
            sineWave(numcycles,BSEtype, f)
        if(args.APPSWaveForm == "R"):
            RandomVals(numcycles,BSEtype,f)

    #Defined in ThrottleValue Simulator 
    #Throttle_Value_Simulator.sendValsFromFile('csv_file.csv') #uncomment this line to send values to VCU
            

        


        
    

    '''

with open('csv_file.csv', 'r') as f:

    filecontent = f.read()

    for word in filecontent.split('\n'):
        if (word == '\n'):
            print("what")
        #word = word.strip('\n')
        #print(word.encode('utf-8'))
        print(word)

    # create the csv writer

'''









