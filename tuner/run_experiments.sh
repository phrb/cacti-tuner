#! /bin/bash

set -o xtrace

path="./results/target_area_times_acct_900"
runs=8
duration=900

for memtype in "ram" "cache" "main-memory"; do
    for i in $(seq 1 $runs); do
        ~/julia/julia tuner.jl --path $path/$memtype/$i --mem_type $memtype --duration $duration
    done
done
