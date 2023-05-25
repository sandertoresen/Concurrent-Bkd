
import matplotlib.pyplot as plt
from matplotlib import rcParams


# define THREAD_BUFFER_SIZE 4096
# test small Buffer size
# 64 trees
# Run 1: 58.475401 s
# Run 2: 39.037989 s
# Run 3: 39.248220 s
# Run 4: 39.364338 s
# Run 5: 39.608335 s
# Run 6: 39.804307 s
# Run 7: 39.992638 s

average_time = [
    58.475401,
    39.037989,
    39.248220,
    39.364338,
    39.608335,
    39.804307,
    39.992638]


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
plt.plot(x, average_time, color='blue', marker='o', linestyle='dashed')
plt.show()
