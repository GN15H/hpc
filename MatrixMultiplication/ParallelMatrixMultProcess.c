#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)
#define MAX_VALUE 5

struct shm_data{
  int counter;
  pthread_mutex_t lock;
};

void printMatrix(int size, int matrix[size][size]);
void fillMatrixWithRandoms(int size, int matrix[size][size]);
void calculateMatrixCell(int size, int row, int col, int A[size][size], int B[size][size], int result[size][size]);
void shm_data_init(struct shm_data* shmp);
int parse_arg_data(int argc, char **argv, int* matrixSize, int* processAmount);
void shm_variables_init(int* fd, struct shm_data** shmp);
void shm_matrices_init(int* matrixFd, int** shm_mat, unsigned int matrixSize);
void matrixSegmentCalculation(int size, int pos, int amount, int A[size][size], int B[size][size], int result[size][size]);

int main(int argc, char** argv){

  int matrixSize = 0;
  int processAmount = 0;
  int parseError = parse_arg_data(argc, argv, &matrixSize, &processAmount);
  if(parseError) return 1;
  srand(time(NULL));

  struct timespec start, end;
  struct shm_data* shmp;
  int* shm_mat;
  int fd, matrixFd;
  pid_t pid[processAmount];
  int parentId = getpid();

  shm_variables_init(&fd, &shmp);
  shm_matrices_init(&matrixFd, &shm_mat, matrixSize);
  shm_data_init(shmp);

  int (*A)[matrixSize] = (int(*)[matrixSize]) shm_mat;
  int (*B)[matrixSize] = (int(*)[matrixSize]) (shm_mat + (matrixSize*matrixSize));
  int (*C)[matrixSize] = (int(*)[matrixSize]) (shm_mat + (2*matrixSize*matrixSize));

  fillMatrixWithRandoms(matrixSize, A);
  fillMatrixWithRandoms(matrixSize, B);

  int amount = (matrixSize*matrixSize)/(processAmount+1);

  for (int i = 0; i < processAmount && parentId == getpid(); i++) pid[i] = fork();

  int pos = 0;
  pthread_mutex_lock(&(shmp->lock));
  pos = (shmp->counter)*amount;
  shmp->counter++;
  pthread_mutex_unlock(&(shmp->lock));

  if(parentId == getpid()) 
    clock_gettime(CLOCK_MONOTONIC, &start);

  matrixSegmentCalculation(matrixSize, pos, amount, A, B, C);

  for(int i=0; i<processAmount && parentId == getpid(); i++) waitpid(pid[i], NULL, 0);

  if(parentId == getpid() && (matrixSize*matrixSize)%(processAmount+1)) {
    for(int i=0; i<processAmount; i++)
      calculateMatrixCell(matrixSize, matrixSize-1, matrixSize-1-i,A, B, C);
  }

  if(parentId == getpid()) {
    // printMatrix(matrixSize,C);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,", elapsed);
    shm_unlink("/shm_data");
    shm_unlink("/matrices");
  }

  return 0;
}

void shm_variables_init(int* fd, struct shm_data** shmp){
  *fd = shm_open("/shm_data", O_CREAT | O_RDWR | O_EXCL, 0600);
  if (*fd == -1) errExit("shm_open");
  if (ftruncate(*fd, sizeof(struct shm_data)) == -1) errExit("ftruncate");

  *shmp = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
  if (*shmp == MAP_FAILED) errExit("mmap");
}

void shm_matrices_init(int* matrixFd, int** shm_mat, unsigned int matrixSize){

  *matrixFd = shm_open("/matrices", O_CREAT | O_RDWR | O_EXCL, 0666);
  if (*matrixFd == -1) errExit("shm_open");
  if(ftruncate(*matrixFd, 3 * matrixSize * matrixSize * sizeof(int)) == -1) errExit("ftruncate");   // space for 3 matrices
  *shm_mat = mmap(NULL, 3 * matrixSize * matrixSize * sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED, *matrixFd, 0);
  if(*shm_mat == MAP_FAILED) errExit("mmap");
}

int parse_arg_data(int argc, char **argv, int* matrixSize, int* processAmount){
  if (argc < 3){
      return 1;
  }      
  if(!atoi(argv[1]) || !atoi(argv[2])){
    printf("Invalid parameters");
    return 1;
  }
  *matrixSize = atoi(argv[1]);
  *processAmount = atoi(argv[2]) - 1;
  return 0;
}


void shm_data_init(struct shm_data* shmp){
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(shmp->lock), &attr);
  pthread_mutexattr_destroy(&attr);
  shmp->counter = 0;
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

void fillMatrixWithRandoms(int size, int matrix[size][size]){
    for (int i = 0; i < size; i++)    
        for (int j = 0; j < size; j++)
            matrix[i][j] = rand() % MAX_VALUE;
}

void printMatrix(int size, int matrix[size][size]){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            printf("%i, ", matrix[i][j]);
        }
        printf("\n");
    }
}