#!/bin/bash
# Script: serial.sh
# Ejecuta el programa serial y guarda resultados en csv/serial.csv

BIN="../build/serial"
OUT="../csv/serial.csv"
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
