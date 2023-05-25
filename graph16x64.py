
import matplotlib.pyplot as plt
from matplotlib import rcParams


# 64 trees
# 4096 * 16
# Run 1: 5.811821 s
# Run 2: 4.265906 s
# Run 3: 3.875301 s
# Run 4: 3.642500 s
# Run 5: 3.530235 s
# Run 6: 3.415571 s
# Run 7: 3.308656 s

average_time = [
    5.811821*100,
    4.265906*100,
    3.875301*100,
    3.642500*100,
    3.530235*100,
    3.415571*100,
    3.308656*100]


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
    "Inserting 64 trees without global structures with 65536 data nodes")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='red', marker='x', linestyle='dashed')
plt.show()
