import matplotlib.pyplot as plt
from matplotlib import rcParams
import numpy as np
# 1,2,4,8,16,32,64,128
# 128 trees

# ----program16x128------
# Average thread 1: 7.1922939999999995s
# Average thread 2: 3.6899141s
# Average thread 4: 1.9195742s
# Average thread 8: 1.0436703s
# Average thread 16: 0.6607157s
# Average thread 32: 0.7828791s
# Average thread 64: 1.6834569s
# Average thread 128: 2.7418507s
# ----program4x128------
# Average thread 1: 3.3211765s
# Average thread 2: 1.6806829s
# Average thread 4: 0.8571768000000001s
# Average thread 8: 0.44794329999999993s
# Average thread 16: 0.253241s
# Average thread 32: 0.28125269999999997s
# Average thread 64: 0.5385058s
# Average thread 128: 0.8008262s
# ----program64x128------
# Average thread 1: 63.676514s
# Average thread 2: 32.2869878s
# Average thread 4: 16.437110099999998s
# Average thread 8: 8.4308431s
# Average thread 16: 4.8972808s
# Average thread 32: 5.134210599999999s
# Average thread 64: 8.0275606s
# Average thread 128: 13.3037642s

x = [1, 2, 4, 8, 16, 32, 64, 128]

# 4

y_1 = [
    3.3211765,
    1.6806829,
    0.8571768000000001,
    0.44794329999999993,
    0.253241,
    0.28125269999999997,
    0.5385058,
    0.8008262]

# 16
y_2 = [
    7.1922939999999995,
    3.6899141,
    1.9195742,
    1.0436703,
    0.6607157,
    0.7828791,
    1.6834569,
    2.7418507]

# 64
y_3 = [
    63.676514,
    32.2869878,
    16.437110099999998,
    8.4308431,
    4.8972808,
    5.134210599999999,
    8.0275606,
    13.3037642]

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
plt.title("Runtimes for creating 128 trees of different sizes when ran on server")

plt.plot(np.log2(x), y_1, marker='^', linestyle='-', label="4096 * 4")
plt.plot(np.log2(x), y_2, marker='s', linestyle='-', label="4096 * 16")
plt.plot(np.log2(x), y_3, marker='v', linestyle='-', label="4096 * 64")

# Set the x-tick labels to be powers of 2
ticks = [1, 2, 4, 8, 16, 32, 64, 128]
plt.xticks(np.log2(ticks), ticks)

plt.legend()
plt.show()
