
import matplotlib.pyplot as plt
from matplotlib import rcParams


# #define THREAD_BUFFER_SIZE 4096 * 4
# #define GLOBAL_B_CHUNK_SIZE 16
# 64 tree glob buf 16 thread size 16384
# programBig: 1 - 57.368606899999996
# programBig: 2 - 38.497728
# programBig: 3 - 38.495173199999996
# programBig: 4 - 38.632534799999995
# programBig: 5 - 38.6962396
# programBig: 6 - 38.7062452
# programBig: 7 - 38.811803100000006

average_time = [
    57.368606899999996,
    38.497728,
    38.495173199999996,
    38.632534799999995,
    38.6962396,
    38.7062452,
    38.811803100000006]


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
    "Inserting trees using global structures of size 16 with 16384 thread buffer")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='green', marker='d', linestyle='dashed')
plt.show()
