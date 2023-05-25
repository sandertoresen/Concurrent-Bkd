
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 128 trees
# 4096 * 16
# Run 1: 11.680836 s
# Run 2: 7.876719 s
# Run 3: 7.545828 s
# Run 4: 7.240083 s
# Run 5: 6.714057 s
# Run 6: 6.475886 s
# Run 7: 6.341246 s

average_time = [
    11.680836*100,
    7.876719*100,
    7.545828*100,
    7.240083*100,
    6.714057*100,
    6.475886*100,
    6.341246*100]


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
plt.ylabel("Average runtime in milliseconds")
plt.title(
    "Inserting 128 trees without global structures with 65536 data nodes")
plt.xticks(x)
plt.plot(x, average_time, color='red', marker='^', linestyle='dashed')
plt.show()
