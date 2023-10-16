import matplotlib.pyplot as plt
import os
import typing
import pandas as pd


PLOT_IMAGE_NAME = "PlottedSimValues.png"
CSV_FILE_NAME = "SimulatedValues.csv"

class PlotGeneration():

    @classmethod
    def generate_VCU_plot(cls, plotted_points: dict[str, list[float]], showplot = True):
        """
        Generate the plot png from the VCU simulation values

        :param plotted_points: dictionnary with key as pedal types and values as array of sim voltages
        """

        cls.add_data_points(plotted_points)

        # Add labels and a title

        # Add a legend
        cls.add_lablels("Value Order", 'Voltage (mV)', 'Simulation Data' )

        # Save the graph as a PNG file

        cls.save_image(CSV_FILE_NAME)

        # Show the graph
        if showplot: plt.show()

    
    @classmethod
    def add_data_points(cls, plotted_points) -> tuple:

        marker_list = ['o' for key in plotted_points.keys()]
        label_list = [key for key in plotted_points.keys()]
        Y_points = [value for value in plotted_points.values()]
        X_points = [i for i in range(1,len(Y_points[0])+1)]

        for i in range(len(plotted_points)):
            plt.plot(X_points, Y_points[i], marker = marker_list[i], label = label_list[i])
    
    @classmethod
    def add_lablels(cls, x_axis, y_axis, title):
        plt.xlabel(x_axis)
        plt.ylabel(y_axis)
        plt.title(title)

    @classmethod
    def save_image(cls, file_path : str):
        plt.savefig(PLOT_IMAGE_NAME)


class CsvGeneration():

    @classmethod
    def write_to_csv(cls, data: dict):
        df = pd.DataFrame(data)

        # Define the CSV file name
        csv_file = CSV_FILE_NAME

        # Write the DataFrame to a CSV file
        df.to_csv(csv_file, index=False)

        print(f"Data has been written to {csv_file}")


class DataGeneration(CsvGeneration, PlotGeneration):

    def __init__(self) -> None:
        super().__init__()


