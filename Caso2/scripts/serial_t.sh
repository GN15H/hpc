#!/bin/bash
BIN="../build/serial_t"
OUT="../csv/serial_t.csv"
SIZES=(400 800 1600 3200)
REPS=8

echo "400,800,1600,3200" >"$OUT"

for ((rep = 1; rep <= REPS; rep++)); do
  row=()
  for size in "${SIZES[@]}"; do
    result=$($BIN $size)
    row+=("$result")
  done
  echo "${row[*]}" | sed 's/ /,/g' >>"$OUT"
done
