
import matplotlib.pyplot as plt
from matplotlib import rcParams


# ----program4x64------
# Inserter threads 1: 1.3682592999999998s
# Inserter threads 2: 0.7939739s
# Inserter threads 3: 0.5946260999999999s
# Inserter threads 4: 0.5302686999999999s
# Inserter threads 5: 0.4961925s
# Inserter threads 6: 0.46090090000000006s
# Inserter threads 7: 0.4541334s

average_time = [
    1.3682592999999998*100,
    0.7939739*100,
    0.5946260999999999*100,
    0.5302686999999999*100,
    0.4961925*100,
    0.46090090000000006*100,
    0.4541334*100]


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
