#include <mpi/mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>
#define MASTER 0
#define MASTER_DISTRIBUTE 0x0
#define MASTER_MERGE 0x1
#define NODE_INTERCOMM 0x80

unsigned long long int init_arr(char *arr, unsigned long long int size);
void print_arr(char *arr, unsigned long long int size);

void master_job(unsigned long long int size, unsigned long long int chunk_size,
                int w_size);
void slave_job(unsigned long long int loops, unsigned long long int chunk_size);

int main(int argc, char **argv) {
  int rank, w_size;
  if (argc < 3)
    return -1;
  // srand(time(NULL));
  srand(3000);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &w_size);
  if (w_size == 1) {
    printf("Not enough nodes");
    return -1;
  }
  unsigned long long int size = atoi(argv[1]);
  unsigned long long int loops = atoi(argv[2]);
  unsigned int proc_amount = atoi(argv[3]);
  unsigned long long int chunk_size = size / (w_size - 1);

  if (rank == 0)
    master_job(size, chunk_size, w_size);
  else
    slave_job(loops, chunk_size);
}

unsigned long long int init_arr(char *arr, unsigned long long int size) {
  unsigned long long int cars = 0;
  for (unsigned long long int i = 0; i < size; i++)
    if (rand() % 2) {
      cars += 1;
      arr[i] = 1;
    }
  return cars;
}

void print_arr(char *arr, unsigned long long int size) {
  for (unsigned long long int i = 0; i < size; i++)
    printf("%i", arr[i]);
  printf("\n");
}

void master_job(unsigned long long int size, unsigned long long int chunk_size,
                int w_size) {
  double speed = 0;
  char *arr = (char *)calloc(size + 2, sizeof(char));
  unsigned long long int cars = init_arr(arr + 1, size);
  arr[size + 1] = arr[1];
  arr[0] = arr[size];
  for (unsigned int node = 0; node < w_size - 1; node++) {
    MPI_Send(arr + (node * chunk_size), chunk_size + 2, MPI_CHAR, node + 1,
             MASTER_DISTRIBUTE, MPI_COMM_WORLD);
  }
  MPI_Status status;
  for (unsigned int node = 0; node < w_size - 1; node++) {
    MPI_Recv(arr + (node * chunk_size), chunk_size, MPI_CHAR, node + 1,
             MASTER_MERGE, MPI_COMM_WORLD, &status);
  }
}

void slave_job(unsigned long long int loops, unsigned long long int chunk_size) {
  double speed = 0;
  char *old_arr = (char *)malloc((chunk_size + 2) * sizeof(char));
  char *new_arr = (char *)calloc(chunk_size + 2, sizeof(char));

  MPI_Status status;
  MPI_Recv(old_arr, chunk_size, MPI_CHAR, MASTER, MASTER_DISTRIBUTE,
           MPI_COMM_WORLD, &status);
  MPI_Request request;
  bool first_changed = false;
  bool last_changed = false;
  for (unsigned long long int i = 0; i < loops; i++) {
    for (unsigned long long int cell = 0; cell < chunk_size + 1; cell++) {
      if (old_arr[cell]) {
        if (!old_arr[cell + 1])
          new_arr[cell + 1] = 1;
        else
          new_arr[cell] = 1;
      }
    }
  }
}
