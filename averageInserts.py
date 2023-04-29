import subprocess
import re

num_programs = 31

for i in range(1, num_programs+1):
    total_elapsed_time = 0
    program_name = f"./program{i}"
    
    for j in range(10):
        output = subprocess.check_output(program_name, shell=True).decode("utf-8")
        elapsed_time_str = re.search(r"Time elapsed (\d+\.\d+)s", output).group(1)
        elapsed_time = float(elapsed_time_str)
        total_elapsed_time += elapsed_time
    
    avg_elapsed_time = total_elapsed_time / 10
    print(f"program {i}: average run time {avg_elapsed_time:.2f} seconds!")