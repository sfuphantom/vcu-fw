import matplotlib.pyplot as plt
import os

# Sample data for three datasets
x_values1 = [1, 2, 3, 4, 5]
y_values1 = [10, 25, 18, 30, 12]

x_values2 = [1, 2, 3, 4, 5]
y_values2 = [15, 20, 12, 25, 10]

x_values3 = [i for i in range(1,100,20)]
y_values3 = [5, 8, 14, 22, 300]

# Create a plot with lines and markers for each dataset
plt.plot(x_values1, y_values1, marker='o', label='Dataset 1')
plt.plot(x_values2, y_values2, marker='x', label='Dataset 2')
plt.plot(x_values3, y_values3, marker='^', label='Dataset 3')

# Add labels and a title
plt.xlabel('X-axis Label')
plt.ylabel('Y-axis Label')
plt.title('Multiple Sets of Points Connected by Lines')

# Add a legend
plt.legend()

# Save the graph as a PNG file
plt.savefig('multiple_datasets_plot.png')

# Show the graph
plt.show()

# Delete the file after it's created to avoid memory

#Will be used for plotting data for the framework and being written to the simulation folder. 
if os.path.exists('multiple_datasets_plot.png'):
    os.remove('multiple_datasets_plot.png')
