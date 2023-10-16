from generate_simulation import Simulation, VCU_Pedal, VCU_Pedals, CSV_FILE_NAME
from UploadDrive import VcuGDriveInterface
from vcu_simulation import VCUSimulation, ResponseVCU
from StatesAndEvents import ResponseVCU, EventData, StateData

#pip install pandas
import pandas as pd

class VCUSimInterface:
    def __init__(self) -> None:
        """
        Initialize all components for simulation communication
        """

        self.simulation : Simulation = Simulation()
        self.vcu_gdrive_interface : VcuGDriveInterface = VcuGDriveInterface()
        # self.vcu_writer = VCUSimulation("COM3")

        self.configure_device()

    def configure_device(self):
        """
        configure 
        """
        json_config: dict = self._parse_json(None)
        self.set_metadata(**json_config)

    def _parse_json(self, path_to_json: str) -> dict:
        """
        parse the JSON containing the device info such as 
        """
        return dict()

    def set_metadata(self, **kwargs):
        """
        Configure the simulation and google drive to a specific hardware device
        """
        pass

    def begin(self):
        simulation_res : bool = self.simulation.begin()
        if (simulation_res):

            write_res = self.write_data()
            self.write_to_csv(write_res)

            self.vcu_gdrive_interface.upload_data()
            exit()
        else:
            raise Exception("Uncaught Simulation generation")

    def write_data(self) -> dict:
        sim_model : dict[VCU_Pedals, list[float]] = self.simulation.plotted_points        
        #all values of the dictionnary should be the same lenght
        sim_len = len(sim_model[VCU_Pedals.APPS1])


        res_data = {StateData.__name__ : [], EventData.__name__: []}

        #retrieve data from response
        for sim_num in range(sim_len):

            apps1: float = sim_model[VCU_Pedals.APPS1][sim_num]
            apps2: float = sim_model[VCU_Pedals.APPS2][sim_num]
            bse: float = sim_model[VCU_Pedals.BSE][sim_num]

            res_data[EventData.__name__].append("EVENT")
            res_data[StateData.__name__].append("STATE")


            # response_vcu: ResponseVCU = ResponseVCU(self.vcu_writer.write(int(apps1), int(apps2), int(bse)))

            # res_data[EventData.__name__].append(response_vcu.events)
            # res_data[StateData.__name__].append(response_vcu.state)

        #concatonate the two dictionnaries to be written into 
        merged_data = {**sim_model, **res_data}

        return merged_data


    
    def write_to_csv(self, data: dict):
        df = pd.DataFrame(data)

        # Define the CSV file name
        csv_file = CSV_FILE_NAME

        # Write the DataFrame to a CSV file
        df.to_csv(csv_file, index=False)

        print(f"Data has been written to {csv_file}")

    def verify_sim_model(self):
        """
        Verify that all componenets have the same simulation model lenght
        """
        pass


if __name__ == "__main__":
    interface: VCUSimInterface = VCUSimInterface().begin()




