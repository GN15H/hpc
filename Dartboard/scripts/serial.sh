#!/bin/bash

# Directorio de salida
OUTPUT_DIR="../csv"
mkdir -p "$OUTPUT_DIR"

# Ejecutables
SERIAL="./dartboard_serial"
SERIALO3="./dartboard_serialO3"

# Valores de precision
PRECISIONS=(1000 10000 100000 1000000 10000000)

# Número de repeticiones
REPS=10

# Archivos CSV
CSV_SERIAL="$OUTPUT_DIR/serial.csv"
CSV_SERIALO3="$OUTPUT_DIR/serialO3.csv"

# Compilar programas
echo "Compilando programas..."
gcc -o dartboard_serial ../dartboard_serial.c
gcc -o dartboard_serialO3 ../dartboard_serial.c -O3

# Inicializar archivos CSV
echo "repetition,precision,output" >"$CSV_SERIAL"
echo "repetition,precision,output" >"$CSV_SERIALO3"

echo "Iniciando pruebas..."

# Función para ejecutar pruebas
run_tests() {
  local program=$1
  local csv_file=$2
  for ((rep = 1; rep <= REPS; rep++)); do
    for precision in "${PRECISIONS[@]}"; do
      echo "Ejecutando $program con precision=$precision (repetition=$rep)..."
      output=$($program "$precision")
      echo "$rep,$precision,$output" >>"$csv_file"
    done
  done
}

# Ejecutar para serial y serialO3
run_tests "$SERIAL" "$CSV_SERIAL"
run_tests "$SERIALO3" "$CSV_SERIALO3"

echo "Pruebas completadas. Resultados guardados en $OUTPUT_DIR"
