
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 128 trees
# 4096 * 4
# Run 1: 2.713562 s
# Run 2: 1.864234 s
# Run 3: 1.843674 s
# Run 4: 1.753044 s
# Run 5: 1.665990 s
# Run 6: 1.586219 s
# Run 7: 1.562252 s

average_time = [
    26.628726*100,
    20.437029*100,
    16.530267*100,
    15.281646*100,
    14.876902*100,
    14.269012*100,
    13.903820*100]


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
    "Inserting 128 trees without global structures with 16384 data nodes")
# the 20 worst runs are bulk loadings
plt.xticks(x)
plt.plot(x, average_time, color='blue', marker='^', linestyle='dashed')
plt.show()
