
import matplotlib.pyplot as plt
from matplotlib import rcParams

# ----program64x128------
# Inserter threads 1: 52.5305933s
# Inserter threads 2: 30.377571900000003s
# Inserter threads 3: 22.701738699999996s
# Inserter threads 4: 20.0688086s
# Inserter threads 5: 18.414225899999998s
# Inserter threads 6: 17.1812455s
# Inserter threads 7: 16.2787795s

average_time = [
    52.5305933,
    30.377571900000003,
    22.701738699999996,
    20.0688086,
    18.414225899999998,
    17.1812455,
    16.2787795]


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
