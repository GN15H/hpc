#!/bin/bash

BIN="../build/serial_t_O3"
OUT="../csv/serial_t_O3.csv"
SIZES=(400 800 1600 3200)
REPS=8

# Ensure directories exist
mkdir -p "$(dirname "$BIN")"
mkdir -p "$(dirname "$OUT")"

echo "400,800,1600,3200" >"$OUT"

for ((rep = 1; rep <= REPS; rep++)); do
  row=()
  for size in "${SIZES[@]}"; do
    result=$($BIN $size)
    row+=("$result")
  done
  echo "${row[*]}" | sed 's/ /,/g' >>"$OUT"
done
