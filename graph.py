import matplotlib.pyplot as plt
from matplotlib import rcParams

trees = [  # 2,
    4,
    8,
    16,
    32,
    64,
    128,
    256]

part = 1000000

# average_time = [
#     195080/part,
#     102550/part,
#     153400/part,
#     160470/part,
#     162470/part,
#     235620/part,
#     235040/part,
#     314320/part]
# x = range(0, len(average_time))
# plt.xlabel("Kdb-tree size")
# plt.ylabel("Average delete in milliseconds")
# plt.title("Delete timings single trees")
# # the 20 worst runs are bulk loadings
# plt.xticks(x, trees)
# plt.plot(x, average_time, color='green', marker='o', linestyle='dashed')
# plt.show()

# 16384 trees: 2.625375s (16777216 nodes)
# 8192 trees: 2.604198s (16777216 nodes)
# 4096 trees: 2.598557s (16777216 nodes)
# 2048 trees: 2.659225s (16777216 nodes)
# 1024 trees: 2.585117s (16777216 nodes)
# 512 trees: 2.587352s (16777216 nodes)
# 256 trees: 2.592684s (16777216 nodes)
# 128 trees: 2.579538s (16777216 nodes)
# 64 trees: 2.592400s (16777216 nodes)
# 32 trees: 2.532106s (16777216 nodes)
# 16 trees: 2.477229s (16777216 nodes)
# 8 trees: 2.505266s (16777216 nodes)
# 4 trees: 2.472122s (16777216 nodes)
# 2 trees: 2.477080s (16777216 nodes)
# 1 trees: 2.513503s (16777216 nodes)


average_time = [
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
    "Comparing read performance of different tree distributions for full data retrival")
# the 20 worst runs are bulk loadings
plt.xticks(numTrees)
plt.plot(numTrees, average_time, color='blue', marker='o', linestyle='dashed')
plt.xscale('log', base=2)
plt.show()
