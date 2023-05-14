import numpy as np


average_time100 = np.array([
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
    2.513503])


average_time25 = np.array([
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
    0.594504])


average_time10 = np.array([
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
    0.250628])


# Normalize the datasets
average_time10_norm = average_time10 / np.max(average_time10)
average_time25_norm = average_time25 / np.max(average_time25)
average_time100_norm = average_time100 / np.max(average_time100)

# Calculate the correlation matrix
corr_matrix = np.corrcoef(
    [average_time10_norm, average_time25_norm, average_time100_norm])

# Print the correlation matrix
print(corr_matrix)

mean_corr = np.mean(corr_matrix[np.triu_indices(3, k=1)])

print(f"Mean correlation coefficient: {mean_corr:.4f}")
