# *******************************************************************************************************
#  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22
#
#  This program is supplied as is, it is up to the user of the program to decide if the program is
#  suitable for the intended purpose and free from errors.
#
#  This code allows you to plot csv localization data on a map.
# *******************************************************************************************************


import json

import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import linear_sum_assignment
from scipy.spatial.distance import cdist
import matplotlib.ticker as ticker
import pandas as pd
from matplotlib import image

factor = 15.5244  # This is the factor px/m map1 has 15.5244 and map2 has 45.16
analysis = True  # True if you provide a second csv to compare points with
data_from_db_export = True  # True if the data is in json
map_img = 'example-data/map1.jpg'  # A picture of the map that is used as the background
calculated_data_csv = 'example-data/LoRa_map1_exp1.csv'  # The main calculated csv
compare_data_csv = 'example-data/GNSS_map1_exp1.csv'  # The csv of the points that have been compared with

if __name__ == '__main__':
    x_points = []
    y_points = []

    # Reading the calulated csv
    calculated_data = pd.read_csv(calculated_data_csv)

    if (data_from_db_export):
        for location in calculated_data["location"]:
            a = json.loads(location)
            x_points.append(a["x"] * factor)
            y_points.append(a["y"] * factor)
    else:
        calculated_data = np.multiply(calculated_data.values, factor)
        x_points = calculated_data[:, [0]]
        y_points = calculated_data[:, [1]]

    # Reading the real data to compare with:
    compare_data = pd.read_csv(compare_data_csv)
    compare_data = np.multiply(compare_data.values, factor)

    # Analyzing closest point for each calculated point
    a = np.dstack((np.array(x_points), np.array(y_points)))[0]
    C = cdist(compare_data, a)
    assi, assigment = linear_sum_assignment(C)
    N = compare_data.shape[0]
    average = (C[assi, assigment].sum() / N) / factor
    print("The average error on the points is: " + str(round(average, 1)) + "m")

    # Plotting
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    data = image.imread(map_img)

    # Marker for adding it to th legend
    ax1.plot(0, 0, color="red")

    ax1.plot(x_points, y_points, marker='v', color="magenta")

    if analysis:
        ax1.plot(compare_data[:, [0]], compare_data[:, [1]], marker='s', color="yellow", linestyle='None')
        for p in range(N):
            ax1.plot([compare_data[p, 0], a[assigment[p], 0]], [compare_data[p, 1], a[assigment[p], 1]], color="cyan",
                     linestyle="dashed")

    # Changing the scale of the axis to meters
    ticks_x = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(round(x / factor)))
    ax1.xaxis.set_major_formatter(ticks_x)
    ticks_y = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(round(x / factor)))
    ax1.yaxis.set_major_formatter(ticks_y)
    ax1.set_xlabel("distance (m)")
    ax1.set_ylabel('distance (m)')

    # Adding a proper legend
    ax1.legend(['Approximate path', 'Calculated location (LoRa)', 'Actual location (GNSS)',
                'Error with corresponding location'])

    ax1.imshow(data)
    plt.show()
