import subprocess
import re


program_numbers = [1, 2, 4, 8, 16, 30]
program_numbers.reverse()
avg_times = []

for i in program_numbers:
    total_elapsed_time = 0
    program_name = "./program"
    program_arg = str(i)
    for j in range(10):
        print(f"program{i} {j}/10")
        output = subprocess.check_output([program_name, program_arg]).decode("utf-8")
        elapsed_time_str = re.search(r"Time elapsed (\d+\.\d+)s", output).group(1)
        elapsed_time = float(elapsed_time_str)
        total_elapsed_time += elapsed_time

    avg_elapsed_time = total_elapsed_time / 10
    avg_times.append(avg_elapsed_time)
    print(f"program {i}: average run time {avg_elapsed_time:.2f} seconds!")

print("\nAll average times:")
for i, avg_time in enumerate(avg_times):
    print(f"program {i+1} average run time {avg_time:.2f} seconds")