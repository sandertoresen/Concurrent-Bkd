import subprocess
import re

tree_counts = [16384, 8192, 4096, 2048, 1024,
               512, 256, 128, 64, 32, 16, 8, 4, 2, 1]
# tree_counts = [2, 1]
num_runs = 10

results = {}
for count in tree_counts:
    total_time = 0.0
    num_nodes = 0
    for i in range(num_runs):
        command = ['./program', str(count)]
        output = subprocess.check_output(command, universal_newlines=True)
        matches = re.findall(
            r'Read (\d+) values in (\d+\.\d+)s from (\d+) trees', output)
        num_values, time_str, num_trees = matches[0]
        total_time += float(time_str)
        num_nodes = int(num_values)
    avg_time = total_time / num_runs
    results[count] = (avg_time, num_nodes)
    print(f"Avg time for {num_nodes} nodes ({count} trees): {avg_time:.6f}s")

print("Results:")
for count, (avg_time, num_nodes) in results.items():
    print(f"{count} trees: {avg_time:.6f}s ({num_nodes} nodes)")
