#!/bin/bash

# Directorio de salida
OUTPUT_DIR="../csv"
mkdir -p "$OUTPUT_DIR"

# Ejecutables
PARALLEL="./dartboard_parallel"
PARALLELO3="./dartboard_parallelO3"

# Valores de precision
PRECISIONS=(1000 10000 100000 100000 10000000)

# Valores de threads
THREADS=(4 6 8 10 12 14)

# Número de repeticiones
REPS=10

# Archivos CSV
CSV_PARALLEL="$OUTPUT_DIR/parallel.csv"
CSV_PARALLELO3="$OUTPUT_DIR/parallelO3.csv"

# Compilar programas
echo "Compilando programas paralelos..."
gcc -o dartboard_parallel ../dartboard_parallel.c
gcc -o dartboard_parallelO3 ../dartboard_parallel.c -O3

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
