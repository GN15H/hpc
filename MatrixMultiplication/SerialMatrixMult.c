#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define DEFAULT_SIZE 20
#define MAX_VALUE 5

void multiplyMatrices(int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size])
{
    int result = 0;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            result = 0;
            for (int k = 0; k < size; k++)
                result += A[i][k] * B[k][j];
            resultMatrix[i][j] = result;
        }
}

void printMatrix(int size, int matrix[size][size])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%i ,", matrix[i][j]);
        }
        printf("\n");
    }
}

void fillMatrixWithRandoms(int size, int matrix[size][size])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % MAX_VALUE;
        }
    }
}

int main(int argc, char **argv)
{
    struct timespec start, end;
    srand(time(NULL));
    int matrixSize = DEFAULT_SIZE;
    if (argc > 1 && atoi(argv[1]))
        matrixSize = atoi(argv[1]);

    int (*matrixA)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);
    int (*matrixB)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);
    int (*result)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);

    fillMatrixWithRandoms(matrixSize, matrixA);
    fillMatrixWithRandoms(matrixSize, matrixB);
    clock_gettime(CLOCK_MONOTONIC, &start);

    multiplyMatrices(matrixSize, matrixA, matrixB, result);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("%.6f,", elapsed);
    free(matrixA);
    free(matrixB);
    free(result);
    return 0;
}
