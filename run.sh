#!/bin/bash

for j in 1 10 100; do
    echo "Running Batch_size ${j}"
    for i in {1..5}; do
        mpirun -np 13 ./DownpourLogisticRegression data/heart_scale 270 13 ${j} | grep "Took" | tail -n 1 
    done
done
