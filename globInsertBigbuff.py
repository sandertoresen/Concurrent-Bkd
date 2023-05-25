
import matplotlib.pyplot as plt
from matplotlib import rcParams


# #define THREAD_BUFFER_SIZE 4096 * 4
# #define GLOBAL_B_CHUNK_SIZE 16
# Run 1: 58.380312 s
# Run 2: 39.063214 s
# Run 3: 39.306935 s
# Run 4: 39.419243 s
# Run 5: 39.578428 s
# Run 6: 39.787662 s
# Run 7: 40.014436 s

average_time = [
    58.380312,
    39.063214,
    39.306935,
    39.419243,
    39.578428,
    39.787662,
    40.014436]


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
plt.plot(x, average_time, color='green', marker='o', linestyle='dashed')
plt.show()
