#!/bin/bash
BIN="../build/parallel_optimized_O3"
OUT="../csv/parallel_optimized_O3.csv"
SIZES=(400 800 1600 3200)
THREADS=(2 4 8 12)
REPS=8

echo "thread,repetition,matrix_size,output" >"$OUT"

for t in "${THREADS[@]}"; do
  for ((rep = 1; rep <= REPS; rep++)); do
    for size in "${SIZES[@]}"; do
      result=$($BIN $size $t)
      echo "$t,$rep,$size,$result" >>"$OUT"
    done
  done
done
