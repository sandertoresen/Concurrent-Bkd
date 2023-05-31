
import matplotlib.pyplot as plt
from matplotlib import rcParams


# define THREAD_BUFFER_SIZE 4096
# 64 tree glob buf 64 thread size 4096
# programSmall: 1 - 56.9513036
# programSmall: 2 - 38.33518250000001
# programSmall: 3 - 38.4632033
# programSmall: 4 - 38.4854577
# programSmall: 5 - 38.584429199999995
# programSmall: 6 - 38.738613199999996
# programSmall: 7 - 38.8464384

average_time = [
    56.9513036,
    38.33518250000001,
    38.4632033,
    38.4854577,
    38.584429199999995,
    38.738613199999996,
    38.8464384]


x = [
    1, 2, 3, 4, 5, 6, 7
]
# plt.rcParams['font.family'] = 'serif'
# plt.rcParams['font.serif'] = ['Computer Modern']

# Set the font family to Computer Modern Roman
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = ['cmr10']

# Set the font path to the directory where your font files are located
# font_path = '/home/name/.fonts/cm-unicode-0.7.0/cmu-serif-roman.otf'

# Set the font properties for the plot
font_prop = {'family': 'serif', 'size': 12,
             'weight': 'normal', 'style': 'normal'}


# x = range(0, len(average_time))
plt.xlabel("Number of writer threads")
plt.ylabel("Average runtime in seconds")
plt.title(
    "Inserting trees using global structures of size 64 with 4096 thread buffer")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='blue', marker='d', linestyle='dashed')
plt.show()
