
import matplotlib.pyplot as plt
from matplotlib import rcParams


# Delay results:
# 4*4096
# All average times:
# program 30 average run time 37.67 seconds
# program 16 average run time 69.78 seconds
# program 8 average run time 136.20 seconds
# program 4 average run time 271.48 seconds
# program 2 average run time 541.95 seconds
# program 1 average run time 1082.88 seconds

average_time = [
    37.67,
    69.78,
    136.20,
    271.48,
    541.95,
    1082.88
]

average_time.reverse()

x = [
    1, 2, 4, 8, 16, 30
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
plt.xlabel("Number of threads")
plt.ylabel("Average overhead in seconds")
plt.title("Inserting trees using global memory and disk with 1ms API delay")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='blue', marker='o', linestyle='dashed')
plt.show()
