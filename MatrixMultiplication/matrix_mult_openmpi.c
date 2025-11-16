#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DEFAULT_SIZE 20
#define MAX_VALUE 5
#define PROCESS_AMOUNT 5

void printData(unsigned int size, int *datico);

void fillMatrixWithRandoms(int size, int *matrix) {
  for (int i = 0; i < size; i++) {
    matrix[i] = rand() % MAX_VALUE;
  }
}

void fillWithZeros(int size, int *arr) {}

int main(int argc, char *argv[]) {
  int rank, size;
  int matrixSize = DEFAULT_SIZE;
  int processAmount = PROCESS_AMOUNT;
  if (argc > 2 && atoi(argv[1]) && atoi(argv[2])) {
    matrixSize = atoi(argv[1]);
    processAmount = atoi(argv[2]);
  }
  unsigned long long int rowsBatch = matrixSize / processAmount;
  srand(time(NULL));
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (rank == 0) {
    printf("el size %i\n", size);
    int *matrixA = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    int *matrixB = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    int *result = (int *)calloc(matrixSize * matrixSize, sizeof(int));
    for (unsigned long long int i = 0; i < matrixSize * rowsBatch; i++)
      result[i + (size - 1) * matrixSize * rowsBatch] = 0;
    fillMatrixWithRandoms(matrixSize * matrixSize, matrixA);
    fillMatrixWithRandoms(matrixSize * matrixSize, matrixB);
    // printData(matrixSize * matrixSize, matrixA);
    // printData(matrixSize * matrixSize, matrixB);
    // printData(matrixSize * matrixSize, result);
    double start = MPI_Wtime();
    for (int i = 0; i < size - 1; i++) {
      MPI_Send(matrixA + (matrixSize * rowsBatch * i), matrixSize * rowsBatch,
               MPI_INT, i + 1, 0, MPI_COMM_WORLD);
      MPI_Send(matrixB, matrixSize * matrixSize, MPI_INT, i + 1, 1,
               MPI_COMM_WORLD);
    }
    for (unsigned long long int i = 0; i < matrixSize * matrixSize * rowsBatch;
         i++) {
      result[((size - 1) * rowsBatch * matrixSize) + (i / matrixSize)] +=
          matrixA[((size - 1) * rowsBatch * matrixSize) +
                  (matrixSize * (i / (matrixSize * matrixSize))) +
                  (i % matrixSize)] *
          matrixB[i % (matrixSize * matrixSize)];
    }
    MPI_Status status;
    for (int i = 0; i < size - 1; i++) {
      MPI_Recv(result + (matrixSize * rowsBatch * i), matrixSize * rowsBatch,
               MPI_INT, i + 1, 2, MPI_COMM_WORLD, &status);
    }
    double end = MPI_Wtime();
    // printData(matrixSize * matrixSize, result);
    printf("%f", end-start);
  } else {
    int *batchA = (int *)malloc(rowsBatch * matrixSize * sizeof(int));
    int *batchB = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    int *result = (int *)calloc(rowsBatch * matrixSize, sizeof(int));
    // printData(matrixSize*matrixSize, resultInt);
    MPI_Status status;
    MPI_Recv(batchA, matrixSize * rowsBatch, MPI_INT, 0, 0, MPI_COMM_WORLD,
             &status);
    MPI_Recv(batchB, matrixSize * matrixSize, MPI_INT, 0, 1, MPI_COMM_WORLD,
             &status);
    for (unsigned long long int i = 0; i < matrixSize * matrixSize * rowsBatch;
         i++) {
      result[i / matrixSize] +=
          batchA[(matrixSize * (i / (matrixSize * matrixSize))) +
                 i % (matrixSize)] *
          batchB[i % (matrixSize * matrixSize)];
    }
    MPI_Send(result, matrixSize * rowsBatch, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // printData(matrixSize*rowsBatch, resultInt);
  }

  MPI_Finalize();
}

void printData(unsigned int size, int *datico) {
  for (int i = 0; i < size; i++)
    printf("%i ", datico[i]);
  printf("\n");
}
