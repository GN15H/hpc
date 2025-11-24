#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned long long int init_arr(char *arr, unsigned long long int size);
void print_arr(char *arr, unsigned long long int size);

int main(int argc, char **argv) {
  if (argc < 2)
    return -1;
  // srand(time(NULL));
  srand(3000);
  struct timespec start, end;

  unsigned long long int size = atoi(argv[1]);
  unsigned long long int loops = atoi(argv[2]);
  char *old_arr = (char *)calloc(size, sizeof(char));
  char *new_arr = (char *)calloc(size, sizeof(char));
  unsigned long long int cars = init_arr(old_arr, size);

  // printf("0: ");
  // print_arr(old_arr, size);
  double speed = 0;

  /*main loop*/
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (unsigned long long int i = 0; i < loops; i++) {
    speed = 0;
    for (unsigned long long int cell = 0; cell < size - 1; cell++) {
      if (old_arr[cell]) {
        if (!old_arr[cell + 1]) {
          new_arr[cell + 1] = 1;
          speed += 1;
        } else
          new_arr[cell] = 1;
      }
    }
    if (old_arr[size - 1]) {
      if (!old_arr[0])
        new_arr[0] = 1;
      else
        new_arr[size - 1] = 1;
    }
    /*average speed computation*/
    speed /= cars;

    // printf("%fv %lli: ",speed, i + 1);
    // print_arr(new_arr, size);
    memcpy(old_arr, new_arr, size);
    memset(new_arr, 0, size);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("%f", elapsed);
  free(old_arr);
  free(new_arr);
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
