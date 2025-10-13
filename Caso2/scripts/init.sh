#!/bin/bash
# Script maestro: ejecuta todos los scripts de pruebas

echo "== Iniciando pruebas =="

scripts=(
  "serial.sh"
  "serial_O3.sh"
  "serial_t.sh"
  "serial_t_O3.sh"
  "parallel.sh"
  "parallel_O3.sh"
  "parallel_omp.sh"
  "parallel_omp_O3.sh"
  "parallel_optimized.sh"
  "parallel_optimized_O3.sh"
)

for s in "${scripts[@]}"; do
  echo "Ejecutando $s..."
  bash "$(dirname "$0")/$s"
done

echo "== Pruebas finalizadas =="
