import os;
import random;
import argparse
import math
import Throttle_Value_Simulator

import csv

#-------------------------------------------------------------------#
#THESE VARIABLES CAN BE CHANGED DEPENDING ON PEDAL/VCU SPECIFICATIONS

#ranges for petals

maxPercentPressed = 50
minPercentPressed = 0

#ranges for VCU voltage
maxVoltageReading = 5
minVoltageReading = 0

#-------------------------------------------------------------------#

def linearPercentToVoltMapping(percent):
    return ((percent/maxPercentPressed)*maxVoltageReading)


#voltage maps identically to APPPS1
def mapVoltToAPPS1(voltage):
    return voltage

#voltage will map with an offset for APPS2
#values are required to be different for faults

def mapVoltToAPPS2(voltage):
    #To be implemented 
    return voltage

#/|/|/| shape, not /\/\/\/\ like typical triangularWaves
def triangularWave(cycles,f):
    
    writer = csv.writer(f)
    curpercent = 0
    array = [0,0,0]
    #25 total increments, and since maxpedal = 50
    #and the VCU accepts data every 20ms, this would model
    #pressing the pedal down all the way in 0.5 seconds

    # 50/2 = 25
    #25 * 20ms = 500ms = 1/2 second
    increment = 2

    #
    for cycle in range(cycles):
        
        while (curpercent <= maxPercentPressed):
            pedalAngleToVolt = round(linearPercentToVoltMapping(curpercent),3)
            
            APPS1 =  mapVoltToAPPS1(pedalAngleToVolt)
            APPS2 =  mapVoltToAPPS2(pedalAngleToVolt)
            
            
            array[0] = APPS1
            array[1] = APPS2


            #arr[2] = 0 -> currently only testing while break percent is 0
            writer.writerow(array)
            curpercent+= increment

        curpercent = 0


def percentToCosWave(percent):
    
    VerticalScale = -maxPercentPressed/2 #negative so that function increased starting from beggining
    
    HorizontalScale = math.pi/maxPercentPressed #max percentage 50 maps to pi (max value)

    VerticalShift = maxPercentPressed/2 #values cannot be negative
    
    mapping = VerticalScale*(math.cos(percent*HorizontalScale)) + VerticalShift

    return mapping
    

def sineWave(cycles,f):

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
        

def RandomVals(cycles,f):
    
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
            

            
        
        
            
        
        
#pass newline ='' as argument to avoid spaces between excel lines  
if __name__ == "__main__":

    my_parser = argparse.ArgumentParser(description='Arbitrary Wave Form Generator')

    my_parser.add_argument('Cycles',
                           metavar='Cyles',
                           type=int,
                           help=' Range :     0 <= int <= 100')

    my_parser.add_argument('WaveForm',
                           metavar='WaveForm',
                           type=str,
                           help='T = triangular \n S = sinusodial \n R = Random')

    args = my_parser.parse_args()

    
    with open('csv_file.csv', 'w', newline = '') as f:
        
        numcycles = args.Cycles
        
        if (args.WaveForm == "T"):
            triangularWave(numcycles,f)
        if (args.WaveForm == "S"):
            sineWave(numcycles,f)
        if(args.WaveForm == "R"):
            RandomVals(numcycles,f)

    #Defined in ThrottleValue Simulator
    #Throttle_Value_Simulator.sendValsFromFile('csv_file.csv')
            

        


        
    

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









