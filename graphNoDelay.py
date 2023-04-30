import matplotlib.pyplot as plt
from matplotlib import rcParams

part = 1000000

# average_time = [
#     195080/part,
#     102550/part,
#     153400/part,
#     160470/part,
#     162470/part,
#     235620/part,
#     235040/part,
#     314320/part]
# x = range(0, len(average_time))
# plt.xlabel("Kdb-tree size")
# plt.ylabel("Average delete in milliseconds")
# plt.title("Delete timings single trees")
# # the 20 worst runs are bulk loadings
# plt.xticks(x, trees)
# plt.plot(x, average_time, color='green', marker='o', linestyle='dashed')
# plt.show()


# program 31: average run time 32.90 seconds!
# program 30: average run time 32.85 seconds!

# All average times:
# program 16 average run time 49.24 seconds
# program 8 average run time 82.05 seconds
# program 4 average run time 147.69 seconds
# program 2 average run time 278.95 seconds
# program 1 average run time 541.42 seconds


average_time = [
    345,
    231,
    186,
    158,
    241,
    69
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
plt.ylabel("Average runtime in milliseconds")
plt.title("Inserting trees without API delay")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='orange', marker='x', linestyle='dashed')
plt.show()
