#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>   // <-- include OpenMP header
#define DEFAULT_SIZE 20
#define MAX_VALUE 5

void printMatrix(int size, int matrix[size][size]);
void fillMatrixWithRandoms(int size, int matrix[size][size]);
void parallel_multiplyMatrices(int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size]);
void calculateMatrixCell(int size, int row, int col, int A[size][size], int B[size][size], int result[size][size]);

int main(int argc, char **argv){
    srand(time(NULL));
    // srand(0);
    struct timespec start, end;

    if (argc < 3){
        printf("Two parameters size and thread amount must be provided\n");
        return 1;
    }

    int matrixSize = atoi(argv[1]);
    int threadAmount = atoi(argv[2]);
    omp_set_num_threads(threadAmount);   // <-- set number of threads

    int (*matrixA)[matrixSize] = malloc(sizeof(int) * matrixSize * matrixSize);
    int (*matrixB)[matrixSize] = malloc(sizeof(int) * matrixSize * matrixSize);
    int (*result)[matrixSize] = malloc(sizeof(int) * matrixSize * matrixSize);

    if(!matrixA || !matrixB || !result){
        printf("Error allocating heap memory\n");
        return 1;
    }

    fillMatrixWithRandoms(matrixSize, matrixA);
    fillMatrixWithRandoms(matrixSize, matrixB);

    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_multiplyMatrices(matrixSize, matrixA, matrixB, result);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,", elapsed);

    // printMatrix(matrixSize, result);

    free(matrixA);
    free(matrixB);
    free(result);
    return 0;
}

void fillMatrixWithRandoms(int size, int matrix[size][size]){
    for (int i = 0; i < size; i++)    
        for (int j = 0; j < size; j++)
            matrix[i][j] = rand() % MAX_VALUE;
}

// ----------------------------------------
// 🔹 OpenMP parallel multiplication
// ----------------------------------------
void parallel_multiplyMatrices(int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size]){
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int c = 0;
            for (int k = 0; k < size; k++)
                c += A[i][k] * B[j][k];
            resultMatrix[i][j] = c;
        }
    }
}

void printMatrix(int size, int matrix[size][size]){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++)
            printf("%i, ", matrix[i][j]);
        printf("\n");
    }
}

