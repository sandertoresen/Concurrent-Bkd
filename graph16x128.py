
import matplotlib.pyplot as plt
from matplotlib import rcParams

# ----program16x128------
# Inserter threads 1: 5.9408216s
# Inserter threads 2: 3.4642013s
# Inserter threads 3: 2.6045589s
# Inserter threads 4: 2.3234889s
# Inserter threads 5: 2.1604322s
# Inserter threads 6: 2.0133955s
# Inserter threads 7: 1.9324818000000001s

average_time = [
    5.9408216*100,
    3.4642013*100,
    2.6045589*100,
    2.3234889*100,
    2.1604322*100,
    2.0133955*100,
    1.9324818000000001*100]


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
