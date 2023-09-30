import matplotlib.pyplot as plt
import os



def create_plot(APPS1_points : list, APPS2_points:list, BSE_points: list, time_points : list):

    # Create a plot with lines and markers for each dataset
    plt.plot(time_points,APPS1_points, marker='o', label='APPS1')
    plt.plot(time_points,APPS2_points, marker='x', label='APPS2')
    plt.plot(time_points,BSE_points, marker='^', label='BSE')

    # Add labels and a title
    plt.xlabel('Value order')
    plt.ylabel('Voltage (V)')
    plt.title('Simulation Data')

    # Add a legend
    plt.legend()

    # Save the graph as a PNG file
    plt.savefig('multiple_datasets_plot.png')

    # Show the graph
    plt.show()

    # Delete the file after it's created to avoid memory

    #Will be used for plotting data for the framework and being written to the simulation folder. 
    # if os.path.exists('multiple_datasets_plot.png'):
    #     os.remove('multiple_datasets_plot.png')
