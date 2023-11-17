#Author : Kevin Litvin
#Date November 2023
#Description : Data storage and generation for CSV data logs and PNG  simulation plots

import matplotlib.pyplot as plt
import os
import typing
import pandas as pd
from vcu_simulation import VCUSimulation


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
        X_points = [i*(VCUSimulation.LATENCY) for i in range(1,len(Y_points[0])+1)]

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


# import matplotlib.pyplot as plt
# import random
# from matplotlib.animation import FuncAnimation
# from scipy.interpolate import interp1d
# import numpy as np

# x_vals = []
# y_vals = []

# fig, ax = plt.subplots()
# line, = ax.plot([], lw=2)

# def init():
#     ax.set_xlim(0, 1)
#     ax.set_ylim(0, 1)
#     return line,

# def animate(i):
#     x = random.random()
#     y = random.random()
#     x_vals.append(x)
#     y_vals.append(y)

#     if len(x_vals) > 3:
#         f = interp1d(x_vals, y_vals, kind='cubic')
#         x_smooth = np.linspace(min(x_vals), max(x_vals), 100)
#         y_smooth = f(x_smooth)
#     else:
#         x_smooth = x_vals
#         y_smooth = y_vals

#     line.set_data(x_smooth, y_smooth)
#     return line,

# ani = FuncAnimation(fig, animate, init_func=init, frames=200, interval=100, blit=True)
# plt.show()

# import matplotlib.pyplot as plt
# from matplotlib.animation import FuncAnimation
# import random
# import time

# class MultiLinePlot:
#     def __init__(self, num_lines):
#         self.num_lines = num_lines
#         self.x_vals = [[] for _ in range(num_lines)]
#         self.y_vals = [[] for _ in range(num_lines)]

#         self.fig, self.ax = plt.subplots()
#         self.lines = [self.ax.plot([], lw=2)[0] for _ in range(num_lines)]

#         self.ax.set_xlim(0, 1)
#         self.ax.set_ylim(0, 1)

#     def init(self):
#         return self.lines

#     def animate(self, i, values_list):
#         for j in range(self.num_lines):
#             self.x_vals[j].append(i / 100.0)
#             value = values_list[j][i]
#             self.y_vals[j].append(value)

#         # Find the shortest line length
#         min_len = min(len(self.x_vals[j]) for j in range(self.num_lines))

#         x_smooth = [self.x_vals[0][i] for i in range(min_len)]
#         y_smooth = [self.y_vals[j][i] for j in range(self.num_lines) for i in range(min_len)]

#         for j in range(self.num_lines):
#             time.sleep(0.033)
#             self.lines[j].set_data(x_smooth, y_smooth[j * min_len:(j + 1) * min_len])

#         return self.lines

#     def run_animation(self, values_list):
#         ani = FuncAnimation(self.fig, self.animate, fargs=(values_list,), init_func=self.init,
#                             frames=len(values_list[0]), interval=100, blit=True)
#         plt.show()

# # Example usage
# num_lines = 3
# plotter = MultiLinePlot(num_lines)
# values_list = [[random.random() for _ in range(200)] for _ in range(num_lines)]
# plotter.run_animation(values_list)
