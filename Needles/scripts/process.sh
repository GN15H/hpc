#!/bin/bash

# Directorio de salida
OUTPUT_DIR="../csv"
mkdir -p "$OUTPUT_DIR"

# Ejecutables
PROCESS="./needles_parallel_process"
PROCESSO3="./needles_parallel_processO3"

# Valores de precision
PRECISIONS=(1000 10000 100000 100000 10000000)

# Valores de procesos
PROCESSES=(4 6 8 10 12 14)

# Número de repeticiones
REPS=10

# Archivos CSV
CSV_PROCESS="$OUTPUT_DIR/process.csv"
CSV_PROCESSO3="$OUTPUT_DIR/processO3.csv"

# Compilar programas
echo "Compilando programas con procesos..."
gcc -o needles_parallel_process ../needles_parallel_process.c -lm
gcc -o needles_parallel_processO3 ../needles_parallel_process.c -lm -O3

# Inicializar archivos CSV
echo "repetition,process,precision,output" >"$CSV_PROCESS"
echo "repetition,process,precision,output" >"$CSV_PROCESSO3"

echo "Iniciando pruebas con procesos..."

# Función para ejecutar pruebas con procesos
run_tests_process() {
  local program=$1
  local csv_file=$2
  for process in "${PROCESSES[@]}"; do
    for ((rep = 1; rep <= REPS; rep++)); do
      for precision in "${PRECISIONS[@]}"; do
        echo "Ejecutando $program con process=$process, precision=$precision (repetition=$rep)..."
        output=$($program "$precision" "$process")
        echo "$rep,$process,$precision,$output" >>"$csv_file"
      done
    done
  done
}

# Ejecutar para process y processO3
run_tests_process "$PROCESS" "$CSV_PROCESS"
run_tests_process "$PROCESSO3" "$CSV_PROCESSO3"

echo "Pruebas con procesos completadas. Resultados guardados en $OUTPUT_DIR"
