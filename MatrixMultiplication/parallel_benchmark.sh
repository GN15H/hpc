# Script para benchmarking de multiplicación de matrices en paralelo
# Ejecuta para cada tamaño de matriz con 1-10 hilos, 10 repeticiones cada uno

# Crear directorio csv si no existe
mkdir -p csv

# Archivo de salida
OUTPUT_FILE="csv/parallel_benchmark.csv"

# Header del CSV
echo "thread,repetition,matrix_size,output" >$OUTPUT_FILE

# Tamaños de matriz a probar
MATRIX_SIZES=(10 100 200 400 800 1600 3200)

# Cantidad de hilos a probar (1 a 10)
THREAD_COUNTS=(1 2 3 4 5 6 7 8 9 10)

# Cantidad de repeticiones por configuración
REPETITIONS=10

echo "Iniciando benchmark de multiplicación de matrices en paralelo..."
echo "Configuración: ${#MATRIX_SIZES[@]} tamaños x ${#THREAD_COUNTS[@]} hilos x $REPETITIONS repeticiones"
echo "Total de ejecuciones: $((${#MATRIX_SIZES[@]} * ${#THREAD_COUNTS[@]} * REPETITIONS))"
echo ""

# Contador para mostrar progreso
total_executions=$((${#MATRIX_SIZES[@]} * ${#THREAD_COUNTS[@]} * REPETITIONS))
current_execution=0

# Loop principal: para cada cantidad de hilos
for threads in "${THREAD_COUNTS[@]}"; do
  echo "Procesando con $threads hilo(s)..."

  # Para cada repetición
  for rep in $(seq 1 $REPETITIONS); do
    echo "  Repetición $rep..."

    # Para cada tamaño de matriz
    for matrix_size in "${MATRIX_SIZES[@]}"; do
      current_execution=$((current_execution + 1))

      # Ejecutar el programa y capturar solo el tiempo
      execution_time=$(./parallel.exe $matrix_size $threads)

      # Guardar en CSV con el nuevo formato: thread,repetition,matrix_size,output
      echo "$threads,$rep,$matrix_size,$execution_time" >>$OUTPUT_FILE

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
echo "- Cantidad de hilos: ${THREAD_COUNTS[*]}"
echo "- Repeticiones por configuración: $REPETITIONS"
echo "- Total de ejecuciones: $total_executions"
EOF
