import subprocess

configs = [1, 2, 4, 8, 16, 32]
times = {}

for config in configs:
    total_time = 0
    for i in range(10):
        output = subprocess.check_output(['./program', str(config)]).decode()
        time_str = output.split('\n')[1].split(': ')[1]
        time = float(time_str[:-1])
        total_time += time
    avg_time = total_time / 10
    times[config] = avg_time
    print(f"Average time for configuration {config}: {avg_time:.6f}s")

print("\nOverall averages:")
for config, avg_time in times.items():
    print(f"Configuration {config}: {avg_time:.6f}s")
