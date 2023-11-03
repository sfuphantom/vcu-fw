from generate_simulation import Simulation, VCU_Pedal, VCU_Pedals
from UploadDrive import VcuGDriveInterface
from vcu_simulation import VCUSimulation, ResponseVCU
from StatesAndEvents import ResponseVCU, EventData, StateData
from data_generation import DataGeneration
import os
from typing import Union



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
            vcu_writer (VCUSimulation): An instance of the VCUSimulation class for VCU serial communication.

            simulation (Simulation): An instance of the Simulation class for simulation generation

            vcu_gdrive_interface (VcuGDriveInterface): An instance of the VcuGDriveInterface class for interfacing with G Drive Folders

            data_generator (DataGeneration): An reference to the DataGeneration class for data formatting.
        """
        self.vcu_writer = VCUSimulation("COM3")
        self.configure_device(device=self.vcu_writer)

        # Pass in VCUSimulation object as a reference for thread-lock compatibility.
        # Useful if we decide to test manual control while running a sim model
        self.simulation: Simulation = Simulation(self.vcu_writer)
        self.vcu_gdrive_interface: VcuGDriveInterface = VcuGDriveInterface()
        self.data_generator = DataGeneration

    def __init__(self) -> None:
        """
        Initialize all components for simulation communication including
        Drive Communication, VCU serial writing, Simulation Generation, and Data Formatting
        """
        self.vcu_writer = VCUSimulation
        self.configure_device(device = self.vcu_writer)

        #Pass in VCUSimulaiton object as reference for thread-lock compatibility.
        #Useful if we decide to test manual control while running a sim model
        self.simulation : Simulation = Simulation(self.vcu_writer)
        self.vcu_gdrive_interface : VcuGDriveInterface = VcuGDriveInterface()
        self.data_generator = DataGeneration


    def configure_device(self, device : VCUSimulation):
        """
        Configure the device from a json file. Currently unimplemented
        but could be useful for setting baudrate, or the OS ports
        """
        json_config: dict = self._parse_json(None)
        self.set_metadata(device, **json_config)

    def _parse_json(self, path_to_json: str) -> dict:
        """
        Parse the JSON containing the VCU specs
        """
        if path_to_json == None:
            #TODO: implemented .json deserializing if we test VCU's with different hardware specifications
            return dict()
        pass

    def set_metadata(self, device, **config):
        """
        Configure the simulation peripherals based on the config
        """
        #Unimplemented for now
        pass

    def begin(self):
        """
        Initialization point of the simulation interface. Executes the simulation, retrieves the results
        and finally uploads the data to the Google Drive.
        """
        simulation_res : bool = self.simulation.begin()
        if (simulation_res):
            write_res : dict[Union[VCU_Pedal, str], Union[list[int], list[EventData], list[StateData]]] = self._write_data()
            self.data_generator.write_to_csv(write_res)
            self.vcu_gdrive_interface.upload_data()
            exit()
        else:
            #Handle 
            raise Exception("Problem with building simulation")

    def _write_data(self) -> dict[Union[VCU_Pedal, str], Union[list[int], list[EventData], list[StateData]]]:
        """
        Write the generated simulation to the VCU firmware. Retrieve the responses for the events and state changes. 
        """
        
        sim_model : dict[VCU_Pedals, list[float]] = self.simulation.plotted_points  
        #all the lenghts of the sim model are the same  
        sim_len = len(sim_model[VCU_Pedals.APPS1])


        res_data = {StateData.__name__ : [], EventData.__name__: []}

        #retrieve data from response
        for sim_num in range(sim_len):

            apps1: float = sim_model[VCU_Pedals.APPS1][sim_num]
            apps2: float = sim_model[VCU_Pedals.APPS2][sim_num]
            bse: float = sim_model[VCU_Pedals.BSE][sim_num]

            response_vcu: ResponseVCU = ResponseVCU(self.vcu_writer.write(int(apps1), int(apps2), int(bse)))

            res_data[EventData.__name__].append(response_vcu.events)
            res_data[StateData.__name__].append(response_vcu.state)

        #concatonate the two dictionnaries to be written into 
        return {**sim_model, **res_data}


#call this file in python using python3 sim_interface.py 
if __name__ == "__main__":
    interface: VCUSimInterface = VCUSimInterface().begin()




