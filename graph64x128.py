
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 128 trees
# 4096 * 64
# Run 1: 53.127481 s
# Run 2: 37.293918 s
# Run 3: 32.927723 s
# Run 4: 29.668766 s
# Run 5: 28.071584 s
# Run 6: 27.402124 s
# Run 7: 26.708567 s

average_time = [
    53.127481,
    37.293918,
    32.927723,
    29.668766,
    28.071584,
    27.402124,
    26.708567]


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


plt.xlabel("Number of writer threads")
plt.ylabel("Average runtime in seconds")
plt.title(
    "Inserting 128 trees without global structures with 262144 data nodes")
plt.xticks(x)
plt.plot(x, average_time, color='black', marker='^', linestyle='dashed')
plt.show()
