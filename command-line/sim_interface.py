#Author : Kevin Litvin
#Date : November 2023
#Description : Encapsulates VCU serial communication, Drive communication, sim model generation, and data formatting

from generate_simulation import Simulation, VCU_Pedal, VCU_Pedals
from UploadDrive import VcuGDriveInterface
from vcu_communication import VCU_Communication, ResponseVCU
from StatesAndEvents import ResponseVCU, EventData, StateData
from data_generation import DataGeneration
import time
import os
from typing import Union

import json





class VCUSimInterface:
    """
    The VCUSimInterface class serves as the interface for the VCU Test Framework, providing 

    functionality for simulating the VCU's behavior in race-like scenarios. It encompasses 

    exentensive simulation generation features, VCU serial communication, interfacing 

    with Team Phantom's Google Drive Log Folders, and taylored Data Formatting

    ---------------------------USER DOCUMENT FOR USAGE ------------------------------

    https://docs.google.com/document/d/1h_xk5TsREIRYUv_hB6RO2n2hTzEec8QP7Y_7VT4gCtU/

    ---------------------------------------------------------------------------------
    """
    def __init__(self) -> None:
        """
        Initialize all components for simulation communication

         Attributes:
            vcu_writer (VCU_Communication): An instance of the VCU_Communication class for VCU serial communication.

            simulation (Simulation): An instance of the Simulation class for simulation generation

            vcu_gdrive_interface (VcuGDriveInterface): An instance of the VcuGDriveInterface class for interfacing with G Drive Folders

            data_generator (DataGeneration): An reference to the DataGeneration class for data formatting.
        """
        self.vcu_writer = VCU_Communication(None)
        self.simulation: Simulation = Simulation()
        self.vcu_gdrive_interface: VcuGDriveInterface = VcuGDriveInterface()
        self.data_generator = DataGeneration

    def begin(self):
        """
        Initialization point of the simulation interface. Executes the simulation, retrieves the results
        and finally uploads the data to the Google Drive.
        """
        simulation_res : bool = self.get_command()
        if (simulation_res):
            write_res = self._write_data()
            self.data_generator.write_to_csv(write_res)
            self.vcu_gdrive_interface.upload_data()
            exit()
        else:
            #Handle 
            raise Exception("Problem with building simulation")
        
    def get_command(self) -> bool:
        """
        Retrieve user input
        """

        while True:
            args = input(">>>")
            ret = self.simulation._parse_args(args)
            #Simulation Args
            if isinstance(ret, dict):
                self.simulation.add_simulation(ret)
                self._generate_plot(self.simulation.plotted_points, self.simulation.sim_duration)
            #Manual Control Args
            if isinstance(ret, tuple):
                self.execute_manual_control(*ret)
            #Exit Or Help, or Invalid args
            if isinstance(ret, bool):
                if not ret:
                    pass
                if ret:
                    # Successfully wrote values
                    # TODO: Clear VCU plots
                    return True
                
    def execute_manual_control(self, *args):
        """
        Transmit the manual control arguments to the VCU and print the raw reponse
        """
        response = ResponseVCU(self.vcu_writer.write(*args))
        print(str(response))

    def _generate_plot(self, plotted_points: dict, duration : int):
        """
        Display the plot for the current simulation model including the simulation duration
        """
        print("\nClose Plot to Continue")
        self.data_generator.generate_VCU_plot(plotted_points, duration, showplot=True) 

    def _write_data(self) -> dict[str, list[Union[str, float]]]:
        """
        Write the generated simulation to the VCU firmware. 
        Retrieve the responses for the events and state changes. 
        """
        
        sim_model : dict[VCU_Pedals, list[float]] = self.simulation.plotted_points.copy()
        #all the lenghts of the sim model are the same  
        sim_len = len(sim_model[VCU_Pedals.APPS1])

        
        res_data = { StateData.__name__ : [], EventData.__name__: [], "Raw Response": []}

        time_data = {"Time (ms)" : []}

        start_time = time.time()

        #retrieve data from response
        for sim_num in range(sim_len):

            apps1: float = sim_model[VCU_Pedals.APPS1][sim_num]
            apps2: float = sim_model[VCU_Pedals.APPS2][sim_num]
            bse: float = sim_model[VCU_Pedals.BSE][sim_num]

            end_time = time.time()

            response_vcu: ResponseVCU = ResponseVCU(self.vcu_writer.write(int(apps1), int(apps2), int(bse)))

            res_data[EventData.__name__].append(response_vcu.events_str)
            res_data[StateData.__name__].append(response_vcu.state_str)
            res_data["Raw Reponse"] = str(response_vcu)

            # Calculate the relative time passed in milliseconds
            ellapsed_time = round(((end_time - start_time) * 1000),2)
            time_data["Time (ms)"].append(ellapsed_time)

        #format the column names 
        sim_model = {str(pedal.name) + " (mV)" : value for pedal, value in sim_model.items()}
        #concatonate dictionnaries in the order which will be written into the csv file
        return {**time_data,**sim_model, **res_data}


#call this file in python using python3 sim_interface.py 
if __name__ == "__main__":
    interface : VCUSimInterface = VCUSimInterface()
    interface.begin()



