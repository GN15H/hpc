#!/bin/bash

# Directorio de salida
OUTPUT_DIR="../csv"
mkdir -p "$OUTPUT_DIR"

# Ejecutables
PARALLEL="./needles_parallel"
PARALLELO3="./needles_parallelO3"

# Valores de precision
PRECISIONS=(10000 1000000 100000000 10000000000)

# Valores de threads
THREADS=(4 6 8 10 12 14)

# Número de repeticiones
REPS=10

# Archivos CSV
CSV_PARALLEL="$OUTPUT_DIR/parallel.csv"
CSV_PARALLELO3="$OUTPUT_DIR/parallelO3.csv"

# Compilar programas
echo "Compilando programas paralelos..."
gcc -o needles_parallel ../needles_parallel.c -lm
gcc -o needles_parallelO3 ../needles_parallel.c -lm -O3

# Inicializar archivos CSV
echo "repetition,threads,precision,output" >"$CSV_PARALLEL"
echo "repetition,threads,precision,output" >"$CSV_PARALLELO3"

echo "Iniciando pruebas paralelas..."

# Función para ejecutar pruebas paralelas
run_tests_parallel() {
  local program=$1
  local csv_file=$2
  for threads in "${THREADS[@]}"; do
    for ((rep = 1; rep <= REPS; rep++)); do
      for precision in "${PRECISIONS[@]}"; do
        echo "Ejecutando $program con threads=$threads, precision=$precision (repetition=$rep)..."
        output=$($program "$precision" "$threads")
        echo "$rep,$threads,$precision,$output" >>"$csv_file"
      done
    done
  done
}

# Ejecutar para parallel y parallelO3
run_tests_parallel "$PARALLEL" "$CSV_PARALLEL"
run_tests_parallel "$PARALLELO3" "$CSV_PARALLELO3"

echo "Pruebas paralelas completadas. Resultados guardados en $OUTPUT_DIR"
