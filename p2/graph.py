import scienceplots
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys

plt.style.use('science')
# specify figure size (width, height)
fig = plt.figure(figsize=(8, 3))
ax = fig.gca()

ax.tick_params(axis='x', labelsize=28)
ax.tick_params(axis='y', labelsize=28)

data = pd.read_csv('cyclictestURJC.csv')
# data2 = pd.read_csv('noRtHackbench.csv')
# data3 = pd.read_csv('noRtIdle.csv')
filename = sys.argv[1]


# Define a dictionary to map cases to their corresponding actions
if filename == 'NRTB':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='blue',
                          alpha=0.5, edgecolor='black', label="Non Real Time - bonnie++")
    plt.title('cyclictestURJC RaspberryPi - Kernel No RT', fontsize=40)

elif filename == 'NRTH':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='red',
                          alpha=0.5, edgecolor='black', label="Non Real Time - hackbench")
    plt.title('cyclictestURJC RaspberryPi - Kernel No RT', fontsize=40)

elif filename == 'NRTI':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='green',
                          alpha=0.5, edgecolor='black', label="Non Real Time - idle")
    plt.title('cyclictestURJC RaspberryPi - Kernel No RT', fontsize=40)

elif filename == 'RTB':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='blue',
                          alpha=0.5, edgecolor='black', label="Real Time - bonnie++")
    plt.title('cyclictestURJC RaspberryPi - Kernel RT', fontsize=40)


elif filename == 'RTH':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='red',
                          alpha=0.5, edgecolor='black', label="Real Time - hackbench")
    plt.title('cyclictestURJC RaspberryPi - Kernel RT', fontsize=40)

elif filename == 'RTI':
    data['LATENCIA'] = data['LATENCIA'] / 1000
    data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='green',
                          alpha=0.5, edgecolor='black', label="Real Time - idle")
    plt.title('cyclictestURJC RaspberryPi - Kernel RT', fontsize=40)

else:
    print("Error: file not found")
    exit()

plt.legend(loc="upper right", prop={'size': 30})
plt.ylabel('Frecuencia', fontsize=38)
plt.xlabel('latencia (microsegundos)', fontsize=38)
plt.show()
