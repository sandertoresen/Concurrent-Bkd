
import matplotlib.pyplot as plt
from matplotlib import rcParams

# ----program4x128------
# Inserter threads 1: 2.7337909s
# Inserter threads 2: 1.56817s
# Inserter threads 3: 1.1782165999999998s
# Inserter threads 4: 1.0331644999999998s
# Inserter threads 5: 0.9528235s
# Inserter threads 6: 0.8923019999999999s
# Inserter threads 7: 0.8595501000000001s

average_time = [
    2.7337909*100,
    1.56817*100,
    1.1782165999999998*100,
    1.0331644999999998*100,
    0.9528235*100,
    0.8923019999999999*100,
    0.8595501000000001*100]


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
