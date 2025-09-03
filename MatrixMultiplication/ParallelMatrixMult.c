#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#define DEFAULT_SIZE 20
#define MAX_VALUE 5

struct mattrmult_data {
    unsigned int size;
    unsigned int pos;
    unsigned int amount;
    void* A;
    void* B;
    void* result;
};

void multiplyMatrices(int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size]);
void printMatrix(int size, int matrix[size][size]);
void fillMatrixWithRandoms(int size, int matrix[size][size]);
void calculateMatrixCell(int size, int row, int col, int A[size][size], int B[size][size], int result[size][size]);
void matrixSegmentCalculation(int size, int pos, int amount, int A[size][size], int B[size][size], int result[size][size]);
void parallel_multiplyMatrices(unsigned int threadAmount, struct mattrmult_data data[threadAmount], int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size]);
void* mattr_wrapper(void* data);

int main(int argc, char **argv){
    // srand(time(NULL));
    srand(0);
    struct timespec start, end;

    if (argc < 3){
        if(!atoi(argv[1]) || !atoi(argv[2])){
            printf("Invalid parameters");
            return 1;
        }
        printf("Two parameters size and thread amount must be provided");
        return 1;
    }
    int matrixSize = atoi(argv[1]);
    int threadAmount = atoi(argv[2]);
    

    int (*matrixA)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);
    int (*matrixB)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);
    int (*result)[matrixSize] = (int (*)[matrixSize])malloc(sizeof(int) * matrixSize * matrixSize);

    if(!matrixA || !matrixB || !result){
        printf("Error allocating heap memory\n");
        return 1;
    }

    struct mattrmult_data data[threadAmount];
    for(int i=0; i<threadAmount; i++){
        data[i].amount=(matrixSize*matrixSize)/threadAmount;
        data[i].size=matrixSize;
        data[i].A=(void*) matrixA;
        data[i].B=(void*) matrixB; 
        data[i].result=(void*) result;
    }

    fillMatrixWithRandoms(matrixSize, matrixA);
    fillMatrixWithRandoms(matrixSize, matrixB);

    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_multiplyMatrices(threadAmount,data, matrixSize, matrixA, matrixB, result);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("%.6f,", elapsed);
    printMatrix(matrixSize, result);
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

void parallel_multiplyMatrices(unsigned int threadAmount, struct mattrmult_data data[threadAmount], int size, int (*A)[size], int (*B)[size], int (*resultMatrix)[size]){
    // struct mattrmult_data data[threadAmount];
    pthread_t threadList[threadAmount];
    long long int rvalues[threadAmount];

    // struct mattrmult_data data;
    // data.amount=(size*size)/threadAmount;
    // data.size=size;
    // data.A=(void*) A;
    // data.B=(void*) B; 
    // data.result=(void*) resultMatrix;

    int th_err;
    for(int i=0; i<threadAmount; i++){
        data[i].pos=i*data[i].amount;
        th_err = pthread_create(&(threadList[i]), NULL, mattr_wrapper, (void*) &data[i]);
        if(th_err){
            printf("Error creating thread");
            exit(0);
        }
    }

    if((size*size)%threadAmount){
        for(int i=0; i<threadAmount; i++)
            calculateMatrixCell(size, size-1, size-1-i,A, B, resultMatrix);
    }

    for(int i=0; i<threadAmount; i++){
        pthread_join(threadList[i], (void **) &(rvalues[i]));
    }
    
}

void* mattr_wrapper(void* data){
    const struct mattrmult_data* args = (struct mattrmult_data*) data;
    for(int i=0; i<args->amount; i++)
        calculateMatrixCell(args->size, (args->pos+i)/args->size, (args->pos+i)%args->size, args->A, args->B, args->result);
    // matrixSegmentCalculation(args->size, args->pos, args->amount, (int(*)[args->size]) args->A,
    //     (int(*)[args->size]) args->B,(int(*)[args->size]) args->result);
    return NULL;
}

void matrixSegmentCalculation(int size, int pos, int amount, int A[size][size], int B[size][size], int result[size][size]){
    for(int i=0; i<amount; i++)
        calculateMatrixCell(size, (pos+i)/size, (pos+i)%size, A, B, result);
}

void calculateMatrixCell(int size, int row, int col, int A[size][size], int B[size][size], int result[size][size]){
    int c=0;
    for(int i=0; i<size; i++)
        c+=(A[row][i]*B[i][col]);
    result[row][col] = c;
}


void printMatrix(int size, int matrix[size][size]){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            printf("%i, ", matrix[i][j]);
        }
        printf("\n");
    }
}