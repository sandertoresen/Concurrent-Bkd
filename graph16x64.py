
import matplotlib.pyplot as plt
from matplotlib import rcParams


# ----program16x64------
# Inserter threads 1: 5.7161508s
# Inserter threads 2: 3.4784284s
# Inserter threads 3: 2.6117966s
# Inserter threads 4: 2.3167621s
# Inserter threads 5: 2.1509533999999997s
# Inserter threads 6: 2.0501910000000003s
# Inserter threads 7: 1.9232781000000003s

average_time = [
    5.7161508*100,
    3.4784284*100,
    2.6117966*100,
    2.3167621*100,
    2.1509533999999997*100,
    2.0501910000000003*100,
    1.9232781000000003*100]


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
