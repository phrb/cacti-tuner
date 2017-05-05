#! /bin/bash

set -o xtrace

cd ~/.bin

wget https://status.julialang.org/download/linux-x86_64

tar xvf linux-x86_64

rm -r julia_old

mv julia julia_old
mv julia-* julia

rm linux-x86_64
