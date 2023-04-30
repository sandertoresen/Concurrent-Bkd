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

average_time = [
    26465220/part,
    26250380/part,
    26345690/part,
    25632430/part,
    25768070/part,
    26392150/part,
    26432620/part]

# plt.rcParams['font.family'] = 'serif'
# plt.rcParams['font.serif'] = ['Computer Modern']

# Set the font family to Computer Modern Roman
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = ['cmr10']

# Set the font path to the directory where your font files are located
font_path = '/home/name/.fonts/cm-unicode-0.7.0/cmu-serif-roman.otf'

# Set the font properties for the plot
font_prop = {'family': 'serif', 'size': 12, 'weight': 'normal', 'style': 'normal'}


x = range(0, len(average_time))
plt.xlabel("Kdb-tree size")
plt.ylabel("Average delete in milliseconds")
plt.title("Delete timings multiple trees")
# the 20 worst runs are bulk loadings
plt.xticks(x, trees)
plt.plot(x, average_time, color='green', marker='o', linestyle='dashed')
plt.show()