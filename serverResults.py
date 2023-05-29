import matplotlib.pyplot as plt
from matplotlib import rcParams
import numpy as np
# 1,2,4,8,16,32,64,128
# 128 trees

# 4096 * 4
# Run 1: 3.350993 s
# Run 2: 2.195981 s
# Run 4: 3.163219 s
# Run 8: 3.375539 s
# Run 16: 3.552864 s
# Run 32: 2.994555 s
# Run 64: 3.687127 s
# Run 128: 5.358019 s

# 4096 * 16
# Run 1: 14.478703 s
# Run 2: 9.378093 s
# Run 4: 11.925392 s
# Run 8: 13.487836 s
# Run 16: 13.821947 s
# Run 32: 11.534818 s
# Run 64: 14.650274 s
# Run 128: 19.578361 s

# 4096 * 64
# Run 1: 64.137432 s
# Run 2: 39.322142 s
# Run 4: 45.866090 s
# Run 8: 54.039236 s
# Run 16: 56.732973 s
# Run 32: 47.442801 s
# Run 64: 59.377097 s
# Run 128: 92.255755 s
x = [1, 2, 4, 8, 16, 32, 64, 128]

y_1 = [3.350993, 2.195981, 3.163219, 3.375539,
       3.552864, 2.994555, 3.687127, 5.358019]
y_2 = [14.478703, 9.378093, 11.925392, 13.487836,
       13.821947, 11.534818, 14.650274, 19.578361]
y_3 = [64.137432, 39.322142, 45.866090, 54.039236,
       56.732973, 47.442801, 59.377097, 92.255755]

# Set the font family to Computer Modern Roman
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = ['cmr10']

# Set the font path to the directory where your font files are located
# font_path = '/home/name/.fonts/cm-unicode-0.7.0/cmu-serif-roman.otf'

# Set the font properties for the plot
font_prop = {'family': 'serif', 'size': 12,
             'weight': 'normal', 'style': 'normal'}

plt.xlabel("Number of writer threads")
plt.ylabel("Average runtime in seconds")
plt.title("Runtimes for creating 128 trees of different sizes when ran on server")

plt.plot(np.log2(x), y_1, marker='^', linestyle='-', label="4096 * 4")
plt.plot(np.log2(x), y_2, marker='s', linestyle='-', label="4096 * 16")
plt.plot(np.log2(x), y_3, marker='v', linestyle='-', label="4096 * 64")

# Set the x-tick labels to be powers of 2
ticks = [1, 2, 4, 8, 16, 32, 64, 128]
plt.xticks(np.log2(ticks), ticks)

plt.legend()
plt.show()
