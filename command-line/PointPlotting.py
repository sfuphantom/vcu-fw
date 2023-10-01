import matplotlib.pyplot as plt
import os
import typing

PLOT_IMAGE_NAME = "PlottedSimValues.png"

def generate_VCU_plot(plotted_points: dict[str, list[float]], showplot = True):
    """
    Generate the plot png from the VCU simulation values

    :param plotted_points: dictionnary with key as pedal types and values as array of sim voltages
    """

    marker_list = ['o', 'x', '^']
    label_list = [key for key in plotted_points.keys()]
    Y_points = [value for value in plotted_points.values()]
    X_points = [i for i in range(1,len(Y_points[0])+1)]

    for i in range(len(plotted_points)):
        plt.plot(X_points, Y_points[i], marker = marker_list[i], label = label_list[i])

     # Add labels and a title
    plt.xlabel('Value order')
    plt.ylabel('Voltage (V)')
    plt.title('Simulation Data')

    # Add a legend
    plt.legend()

    # Save the graph as a PNG file
    plt.savefig(PLOT_IMAGE_NAME)

    # Show the graph
    if showplot: plt.show()

    # Delete the file after it's created to avoid memory

    #Will be used for plotting data for the framework and being written to the simulation folder. 
    # if os.path.exists('multiple_datasets_plot.png'):
    #     os.remove('multiple_datasets_plot.png')
