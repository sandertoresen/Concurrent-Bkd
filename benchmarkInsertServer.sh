#!/bin/bash

j_values=(1 2 4 8 16 32 64 128)

# Run ./program for each value of j
for j in "${j_values[@]}"
do
    echo $j
    for ((k=1; k<=10; k++))
    do
        ./program $j
    done
done
