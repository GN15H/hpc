matrixmult:
	gcc -o build/serial_t serial_transposed.c
	gcc -o build/serial_t_O3 serial_transposed.c -O3
	gcc -o build/serial SerialMatrixMult.c
	gcc -o build/serial_O3 SerialMatrixMult.c -O3
	gcc -o build/parallel ParallelMatrixMult.c
	gcc -o build/parallel_O3 ParallelMatrixMult.c -O3
	gcc -o build/parallel_optimized parallel_transposed.c
	gcc -o build/parallel_optimized_O3 parallel_transposed.c -O3
