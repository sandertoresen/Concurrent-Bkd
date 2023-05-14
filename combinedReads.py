import matplotlib.pyplot as plt
from matplotlib import rcParams

average_time25 = [
    0.756780,
    0.715626,
    0.702408,
    0.709481,
    0.683561,
    0.677533,
    0.680944,
    0.670564,
    0.662937,
    0.636304,
    0.566949,
    0.597696,
    0.567196,
    0.572004,
    0.594504]

average_time100 = [
    2.625375,
    2.604198,
    2.598557,
    2.659225,
    2.585117,
    2.587352,
    2.592684,
    2.579538,
    2.592400,
    2.532106,
    2.477229,
    2.505266,
    2.472122,
    2.477080,
    2.513503]

average_time10 = [
    0.369479,
    0.329194,
    0.308299,
    0.322715,
    0.298112,
    0.290438,
    0.288922,
    0.288420,
    0.272704,
    0.245995,
    0.233717,
    0.245002,
    0.238365,
    0.245181,
    0.250628
]

normalised = True
if not normalised:
    for i, v in enumerate(average_time25):
        average_time25[i] = v * 100

    for i, v in enumerate(average_time10):
        average_time10[i] = v * 100

    for i, v in enumerate(average_time100):
        average_time100[i] = v * 100

if normalised:
    max_time25 = max(average_time25)
    max_time100 = max(average_time100)
    max_time10 = max(average_time10)

    average_time25 = [v / max_time25 for v in average_time25]
    average_time100 = [v / max_time100 for v in average_time100]
    average_time10 = [v / max_time10 for v in average_time10]

numTrees = [
    16384,
    8192,
    4096,
    2048,
    1024,
    512,
    256,
    128,
    64,
    32,
    16,
    8,
    4,
    2,
    1
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


# Plot the data
plt.xlabel("Number of trees")
plt.ylabel("Normalized query time")
plt.title(
    "Comparing read performance of different tree distributions normalized")
plt.xticks(numTrees)
plt.plot(numTrees, average_time25, color='orange',
         marker='o', linestyle='dashed', label='25% of data')
plt.plot(numTrees, average_time100, color='blue',
         marker='s', linestyle='dashed', label='100% of data')
plt.plot(numTrees, average_time10, color='red',
         marker='^', linestyle='dashed', label='10% of data')
plt.xscale('log', base=2)
plt.legend()
plt.show()
