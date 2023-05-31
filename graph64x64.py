
import matplotlib.pyplot as plt
from matplotlib import rcParams

# ----program64x64------
# Inserter threads 1: 26.315219199999994s
# Inserter threads 2: 15.1275993s
# Inserter threads 3: 11.5288674s
# Inserter threads 4: 10.252782700000001s
# Inserter threads 5: 9.4448834s
# Inserter threads 6: 8.839327s
# Inserter threads 7: 8.5308577s

average_time = [
    26.315219199999994,
    15.1275993,
    11.5288674,
    10.252782700000001,
    9.4448834,
    8.839327,
    8.5308577]


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
