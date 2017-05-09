#! /bin/bash

set xtrace -o

path="./results/target_area_1800"
memtypes=("ram" "cache" "mm")
runs=4
duration=10

for memtype in $memtypes; do
    for i in $(seq 1 $runs); do
        ~/julia/julia tuner.jl --path $path/$memtype/$i --mem_type $memtype --duration $duration
    done
done
