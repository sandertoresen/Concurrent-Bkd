import matplotlib.pyplot as plt
from matplotlib import rcParams

# 16384 trees: 0.756780s (4193320 nodes)
# 8192 trees: 0.715626s (4196764 nodes)
# 4096 trees: 0.702408s (4193544 nodes)
# 2048 trees: 0.709481s (4191409 nodes)
# 1024 trees: 0.683561s (4195088 nodes)
# 512 trees: 0.677533s (4197050 nodes)
# 256 trees: 0.680944s (4192265 nodes)
# 128 trees: 0.670564s (4193904 nodes)
# 64 trees: 0.662937s (4197178 nodes)
# 32 trees: 0.636304s (4191554 nodes)
# 16 trees: 0.566949s (4195545 nodes)
# 8 trees: 0.597696s (4196731 nodes)
# 4 trees: 0.567196s (4192269 nodes)
# 2 trees: 0.572004s (4194708 nodes)
# 1 trees: 0.594504s (4193620 nodes)


average_time = [
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

for i, v in enumerate(average_time):
    average_time[i] = v * 100
# average_time.reverse()

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


# x = range(0, len(average_time))
plt.xlabel("Number of trees")
plt.ylabel("Average query time in milliseconds")
plt.title(
    "Comparing read performance of different tree distributions for partial data retrival")
# the 20 worst runs are bulk loadings
plt.xticks(numTrees)
plt.plot(numTrees, average_time, color='orange',
         marker='o', linestyle='dashed')
plt.xscale('log', base=2)
plt.show()
