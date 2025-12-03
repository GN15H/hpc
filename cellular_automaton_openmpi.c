
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MASTER 0
#define MASTER_DISTRIBUTE 0x0
#define MASTER_MERGE 0x1
#define NODE_INTERCOMM 0x8000000000000000
#define NODE_FIRST_POS 0x0
#define NODE_LAST_POS 0x4000000000000000

unsigned long long int init_arr(char *arr, unsigned long long int size);
void print_arr(char *arr, unsigned long long int size);

void master_job(unsigned long long int size, unsigned long long int chunk_size,
                unsigned long long int loops, int w_size);
void slave_job(int rank, int w_size, unsigned long long int loops,
               unsigned long long int chunk_size);

/*
 * first arg: size of line
 * second arg: loops amount
 * third arg: processes amount
 */

int main(int argc, char **argv) {
  int rank, w_size;
  if (argc < 3)
    return -1;
  srand(time(NULL));
  // srand(2500);
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
  MPI_Barrier(MPI_COMM_WORLD);

  double start_time, end_time;
  if (rank == 0) {
    start_time = MPI_Wtime();
  }

  if (rank == 0)
    master_job(size, chunk_size, loops, w_size);
  else
    slave_job(rank, w_size, loops, chunk_size);
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {
    end_time = MPI_Wtime();
    printf("%f", end_time - start_time);
  }
  MPI_Finalize();
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
                unsigned long long int loops, int w_size) {
  char *arr = (char *)calloc(size + 2, sizeof(char));
  unsigned long long int cars = init_arr(arr + 1, size);
  arr[size + 1] = arr[1];
  arr[0] = arr[size];
  for (unsigned int node = 0; node < w_size - 1; node++) {
    MPI_Send(arr + (node * chunk_size), chunk_size + 2, MPI_CHAR, node + 1,
             MASTER_DISTRIBUTE, MPI_COMM_WORLD);
  }
  MPI_Status status;
  unsigned long long int moves[w_size - 1];
  for (unsigned int node = 0; node < w_size - 1; node++) {
    MPI_Recv(moves + node, 1, MPI_UNSIGNED_LONG_LONG, node + 1, MASTER_MERGE,
             MPI_COMM_WORLD, &status);
  }
  unsigned long long int sum = 0;
  for (unsigned int i = 0; i < w_size - 1; i++)
    sum += moves[i];
  // double speed = sum / (cars * loops);
  // printf("quotient %lli remainder %lli", sum / (cars * loops),
  //        sum % (cars * loops));
}

void slave_job(int rank, int w_size, unsigned long long int loops,
               unsigned long long int chunk_size) {
  unsigned long long int moves = 0;
  char *old_arr = (char *)malloc((chunk_size + 2) * sizeof(char));
  char *new_arr = (char *)calloc(chunk_size + 2, sizeof(char));

  MPI_Status status;
  MPI_Recv(old_arr, chunk_size + 2, MPI_CHAR, MASTER, MASTER_DISTRIBUTE,
           MPI_COMM_WORLD, &status);
  MPI_Request request;
  bool first_changed = false;
  bool last_changed = false;
  for (unsigned long long int i = 0; i < loops; i++) {
    new_arr[chunk_size + 1] = old_arr[chunk_size + 1];
    for (unsigned long long int cell = 0; cell < chunk_size + 1; cell++) {
      if (old_arr[cell]) {
        if (!old_arr[cell + 1]) {
          moves += 1;
          new_arr[cell + 1] = 1;
        } else
          new_arr[cell] = 1;
      }
    }
    first_changed = new_arr[0] ^ old_arr[0];
    moves -= first_changed;
    memcpy(old_arr, new_arr, chunk_size + 2);
    memset(new_arr, 0, chunk_size + 2);

    if(w_size ==2){
      old_arr[0] = old_arr[chunk_size];
      old_arr[chunk_size+1] = old_arr[1];
      continue;
    }
    if (rank > 1 && rank < w_size - 1) {
      MPI_Issend(old_arr + 1, 1, MPI_CHAR, rank - 1,
                 NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Issend(old_arr + chunk_size, 1, MPI_CHAR, rank + 1,
                 NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Recv(old_arr + chunk_size + 1, 1, MPI_CHAR, rank + 1,
               NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &status);
      MPI_Recv(old_arr, 1, MPI_CHAR, rank - 1,
               NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &status);
    } else if (rank == 1) {
      MPI_Issend(old_arr + 1, 1, MPI_CHAR, w_size - 1,
                 NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Issend(old_arr + chunk_size, 1, MPI_CHAR, rank + 1,
                 NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Recv(old_arr + chunk_size + 1, 1, MPI_CHAR, rank + 1,
               NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &status);
      MPI_Recv(old_arr, 1, MPI_CHAR, w_size - 1,
               NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &status);
    } else {
      MPI_Issend(old_arr + 1, 1, MPI_CHAR, rank - 1,
                 NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Issend(old_arr + chunk_size, 1, MPI_CHAR, 1,
                 NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &request);
      MPI_Recv(old_arr + chunk_size + 1, 1, MPI_CHAR, 1,
               NODE_INTERCOMM | NODE_FIRST_POS | i, MPI_COMM_WORLD, &status);
      MPI_Recv(old_arr, 1, MPI_CHAR, rank - 1,
               NODE_INTERCOMM | NODE_LAST_POS | i, MPI_COMM_WORLD, &status);
    }
  }
  MPI_Send(&moves, 1, MPI_UNSIGNED_LONG_LONG, MASTER, MASTER_MERGE,
           MPI_COMM_WORLD);
}
