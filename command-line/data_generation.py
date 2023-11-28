#Author : Kevin Litvin
#Date : November 2023
#Description : Data storage and generation for CSV data logs and PNG  simulation plots

import matplotlib.pyplot as plt
import os
import typing
import pandas as pd
from vcu_communication import VCU_Communication


PLOT_IMAGE_NAME = "PlottedSimValues.png"
CSV_FILE_NAME = "SimulatedValues.csv"

class PlotGeneration():
    """
    This class provides methods for generating plots using Matplotlib.
    """

    @classmethod
    def generate_VCU_plot(cls, plotted_points: dict[object, list[int]], duration: int,  showplot = True):
        """
        Generate a plot PNG from VCU simulation values.

        :param plotted_points: A dictionary with pedal types as keys and lists of simulated voltages as values.
        :param showplot: A boolean indicating whether to display the plot (True) or save it without displaying (False).
        """

        cls.add_data_points(plotted_points)

        # Add labels and a title

        # Add a legend
        cls.add_lablels("Time (ms)", 'Voltage (mV)', f'Simulation Data - {duration} (ms)' )

        # Save the graph as a PNG file

        cls.save_image(PLOT_IMAGE_NAME)

        # Show the graph

        plt.legend()
        if showplot: plt.show()

    
    @classmethod
    def add_data_points(cls, plotted_points : dict[object, list[int]]) -> tuple:
        """
        Add data points to the current plot.

        :param plotted_points: A dictionary with pedal types as keys and lists of simulated voltages as values.

        :return: A tuple containing marker styles, labels, Y points, and X points.
        :rtype: tuple
        """

        marker_list = ['o' for key in plotted_points.keys()]
        label_list = [key for key in plotted_points.keys()]
        Y_points = [value for value in plotted_points.values()]
        X_points = [i*(VCU_Communication.LATENCY) for i in range(1,len(Y_points[0])+1)]

        for i in range(len(plotted_points)):
            plt.plot(X_points, Y_points[i], marker = marker_list[i], label = label_list[i])
    
    @classmethod
    def add_lablels(cls, x_axis : str, y_axis : str, title: str):
        """
        Add labels and a title to the current plot.

        :param x_axis: Label for the X-axis.
        :param y_axis: Label for the Y-axis.
        :param title: Title of the plot.
        """
        plt.xlabel(x_axis)
        plt.ylabel(y_axis)
        plt.title(title)

    @classmethod
    def save_image(cls, file_path : str = None):
        """
        Save an image path locally

        :param: file_path: relative path for the file to save
        """
        if file_path is None:
            file_path = PLOT_IMAGE_NAME
        plt.savefig(file_path)


class CsvGeneration():
    """
    This class provides methods for generating CSV data using pandas.
    """
    @classmethod
    def write_row(cls, row : list[int], file_path: str):
        """
        Write the row to an excel file, useful for saving data piecewise
        in the case of expecting a data fault or program crash

        :param row: row of data to write to 
        :param file_path: relative path of file
        """
        pass

    @classmethod
    def write_to_csv(cls, data: dict[object, list[int]]):
        """
        Write data to a CSV file.

        :param data: A dictionary containing data to be written to the CSV file.
        """
        df = pd.DataFrame(data)

        # Define the CSV file name
        csv_file = CSV_FILE_NAME

        # Write the DataFrame to a CSV file
        df.to_csv(csv_file, index=False)

        print(f"Data has been written to {csv_file}")


class DataGeneration(CsvGeneration, PlotGeneration):
    """
    This class inherits functionality from both CsvGeneration and PlotGeneration and provides combined
    capabilities for generating plots and writing data to CSV files.
    """
    def __init__(self) -> None:
        super().__init__()
