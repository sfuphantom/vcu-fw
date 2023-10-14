from generate_simulation import Simulation
from UploadDrive import VcuGDriveInterface

class VCUSimInterface:
    def __init__(self) -> None:
        self.simulation : Simulation = Simulation()
        self.vcu_gdrive_interface : VcuGDriveInterface = VcuGDriveInterface()
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
        ret = self.simulation.begin()
        print(self.simulation.plotted_points)
        if (ret):
            self.vcu_gdrive_interface.upload_data()
            exit()

if __name__ == "__main__":
    interface: VCUSimInterface = VCUSimInterface().begin()




