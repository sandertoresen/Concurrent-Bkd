
import matplotlib.pyplot as plt
from matplotlib import rcParams

# No delay results 64:
# 64*4096
# program 30 average run time 63.29 seconds
# program 16 average run time 55.95 seconds
# program 8 average run time 53.18 seconds
# program 4 average run time 46.29 seconds
# program 2 average run time 24.56 seconds
# program 1 average run time 30.82 seconds

average_time = [
    63.29,
    55.95,
    53.18,
    46.29,
    24.56,
    30.82
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
plt.title(
    "Inserting trees using large global memory and disk buffers without API delay")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='black', marker='o', linestyle='dashed')
plt.show()
