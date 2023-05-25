
import matplotlib.pyplot as plt
from matplotlib import rcParams


# define THREAD_BUFFER_SIZE 4096 * 4
# 64 trees
# Average times script:
# ./program 1: 1.343175s
# ./program 2: 0.902469s
# ./program 3: 0.865262s
# ./program 4: 0.894758s
# ./program 5: 0.853240s
# ./program 6: 0.818819s
# ./program 7: 0.806757s

average_time = [
    1.343175*100,
    0.902469*100,
    0.865262*100,
    0.894758*100,
    0.853240*100,
    0.818819*100,
    0.806757*100]


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
plt.ylabel("Average runtime in milliseconds")
plt.title(
    "Inserting 64 trees without global structures with 16384 data nodes")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='blue', marker='x', linestyle='dashed')
plt.show()
