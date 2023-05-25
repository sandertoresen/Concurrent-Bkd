#!/bin/bash


# Run ./program 1, 2, 3, 4, 5, 6, and 7 ten times each
for ((j=1; j<=7; j++))
do
    echo $j
    for ((k=1; k<=10; k++))
    do
        ./program $j
    done
done
