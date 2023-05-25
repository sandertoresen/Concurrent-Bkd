
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 64 trees
# 4096 * 64
# Run 1: 26.628726 s
# Run 2: 20.437029 s
# Run 3: 16.530267 s
# Run 4: 15.281646 s
# Run 5: 14.876902 s
# Run 6: 14.269012 s
# Run 7: 13.903820 s

average_time = [
    26.628726,
    20.437029,
    16.530267,
    15.281646,
    14.876902,
    14.269012,
    13.903820]


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
    "Inserting 64 trees without global structures with 262144 data nodes")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='black', marker='x', linestyle='dashed')
plt.show()
