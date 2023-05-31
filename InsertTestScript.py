import subprocess

# Define the programs to run and their configurations
programs = {
    "programSmall": [1, 2, 3, 4, 5, 6, 7],
    "programBig": [1, 2, 3, 4, 5, 6, 7]
}

# Define the number of times to run each configuration
num_runs = 10

# Create a dictionary to store the results
results = {}

# Run the tests for each program
for program, configurations in programs.items():
    results[program] = {}
    for config in configurations:
        elapsed_times = []
        for _ in range(num_runs):
            # Run the program and capture the output
            output = subprocess.check_output(
                [f"./{program}", str(config)]).decode("utf-8")
            # Extract the elapsed time from the output
            elapsed_time = float(output.strip().split(" ")[2][:-1])
            elapsed_times.append(elapsed_time)
        # Calculate the average elapsed time
        average_time = sum(elapsed_times) / num_runs
        # Store the average in the results dictionary
        results[program][config] = average_time
        print(f"{program}: {config} - {average_time}")

# Write the results to a file
with open("results.txt", "w") as file:
    for program, config_results in results.items():
        file.write(f"----{program}------\n")
        for config, average_time in config_results.items():
            file.write(f"Average thread {config}: {average_time}s\n")
