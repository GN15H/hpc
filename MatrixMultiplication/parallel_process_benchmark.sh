#!/bin/bash
# Script para benchmarking de multiplicación de matrices con procesos en paralelo
# Ejecuta para cada cantidad de procesos con diferentes tamaños de matriz, 10 repeticiones cada uno

# Crear directorio csv si no existe
mkdir -p csv

# Archivo de salida
OUTPUT_FILE="csv/parallel_process_benchmark.csv"

# Header del CSV
echo "process,repetition,matrix_size,output" > $OUTPUT_FILE

# Tamaños de matriz a probar
MATRIX_SIZES=(10 100 200 400 800 1600 3200)

# Cantidad de procesos a probar (1 a 10)
PROCESS_COUNTS=(1 2 3 4 5 6 7 8 9 10)

# Cantidad de repeticiones por configuración
REPETITIONS=10

echo "Iniciando benchmark de multiplicación de matrices con procesos en paralelo..."
echo "Configuración: ${#MATRIX_SIZES[@]} tamaños x ${#PROCESS_COUNTS[@]} procesos x $REPETITIONS repeticiones"
echo "Total de ejecuciones: $((${#MATRIX_SIZES[@]} * ${#PROCESS_COUNTS[@]} * REPETITIONS))"
echo ""

# Contador para mostrar progreso
total_executions=$((${#MATRIX_SIZES[@]} * ${#PROCESS_COUNTS[@]} * REPETITIONS))
current_execution=0

# Loop principal: para cada cantidad de procesos
for processes in "${PROCESS_COUNTS[@]}"; do
  echo "Procesando con $processes proceso(s)..."

  # Para cada repetición
  for rep in $(seq 1 $REPETITIONS); do
    echo "  Repetición $rep..."

    # Para cada tamaño de matriz
    for matrix_size in "${MATRIX_SIZES[@]}"; do
      current_execution=$((current_execution + 1))

      # Ejecutar el programa y capturar solo el tiempo
      execution_time=$(./parallel_process.exe $matrix_size $processes)

      # Guardar en CSV con el formato: process,repetition,matrix_size,output
      echo "$processes,$rep,$matrix_size,$execution_time" >> $OUTPUT_FILE

      # Mostrar progreso cada 10 ejecuciones
      if [ $((current_execution % 10)) -eq 0 ]; then
        progress=$((current_execution * 100 / total_executions))
        echo "    Progreso: $current_execution/$total_executions ($progress%)"
      fi
    done
  done
  echo ""
done

echo "Benchmark completado!"
echo "Resultados guardados en: $OUTPUT_FILE"
echo ""
echo "Resumen de ejecuciones:"
echo "- Tamaños de matriz: ${MATRIX_SIZES[*]}"
echo "- Cantidad de procesos: ${PROCESS_COUNTS[*]}"
echo "- Repeticiones por configuración: $REPETITIONS"
echo "- Total de ejecuciones: $total_executions"

