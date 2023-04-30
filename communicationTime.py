import matplotlib.pyplot as plt
from matplotlib import rcParams


# Overall averages:
# Configuration 1: 0.675702s
# Configuration 2: 2.678627s
# Configuration 4: 1.561719s
# Configuration 8: 1.895981s
# Configuration 16: 2.418305s
# Configuration 32: 3.946008s

average_time = [
    0.675702,
    2.678627,
    1.561719,
    1.895981,
    2.418305,
    3.629051
]

# average_time.reverse()

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
plt.title("Growing communication overhead")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='black', marker='o', linestyle='dashed')
plt.show()
