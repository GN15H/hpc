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
#include <stdint.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)

struct process_info {   
    long long int hits;
    char padding[56];
    //padding para evitar false sharing
};

struct shm_data{
  int counter;
  pthread_mutex_t lock;
};

void* approx_pi(void* data);
void shm_variables_init(int* shm_data_fd, struct shm_data** shmp);
void shm_data_init(int* tinfo_fd, struct process_info** tinfo, unsigned int processes);
void shm_lock_init(struct shm_data* shmp);

int main(int argc, char** argv){
    if(argc < 3) return 1;
    if(atoi(argv[1]) == 0 || atoi(argv[2]) == 0) return 1;

    int parentId = getpid();

    int shm_data_fd, pinfo_fd;
    struct shm_data* shmp;
    struct process_info* pinfo;

    struct timespec start, end;

    const unsigned long long int n = atoi(argv[1]);
    const int processes = atoi(argv[2]);
    const double factor = 1.0 / RAND_MAX;
    const unsigned long long int p_hits = n/processes;
    pid_t pid[processes-1];
    
    shm_variables_init(&shm_data_fd, &shmp);
    shm_data_init(&pinfo_fd, &pinfo, processes);
    shm_lock_init(shmp);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0; i<processes-1 && parentId == getpid(); i++) pid[i] = fork();

    int pos=0;
    int hits=0;
    pthread_mutex_lock(&(shmp->lock));
    pos = (shmp->counter);
    shmp->counter++;
    pthread_mutex_unlock(&(shmp->lock));

    double x,y;
    unsigned int seed = time(NULL) ^ (uintptr_t)pthread_self(); // unique seed

    for(int i=0; i<p_hits; ++i){
        x = rand_r(&seed)*factor;
        y = rand_r(&seed)*factor;
        if(x*x + y*y < 1.0)
            ++(pinfo[pos].hits);
    }

    if(parentId != getpid()) return 0;

    for(int i=0; i<processes-1;i++)
        waitpid(pid[i], NULL, 0);

    for(int i=0; i<processes; i++)
        hits += pinfo[i].hits;

    const double pi_approx = (4.0 * hits) / (processes*p_hits); 
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,\n", elapsed);
    printf("%.10lf",pi_approx);
    shm_unlink("/shm_data_dartboard");
    shm_unlink("/pinfo_data");

    return 0;
}


void shm_variables_init(int* fd, struct shm_data** shmp){
  *fd = shm_open("/shm_data_dartboard", O_CREAT | O_RDWR | O_EXCL, 0600);
  if (*fd == -1) errExit("shm_open");
  if (ftruncate(*fd, sizeof(struct shm_data)) == -1) errExit("ftruncate");

  *shmp = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
  if (*shmp == MAP_FAILED) errExit("mmap");
}


void shm_data_init(int* pinfo_fd, struct process_info** pinfo, unsigned int processes){
  *pinfo_fd = shm_open("/pinfo_data", O_CREAT | O_RDWR | O_EXCL, 0666);
  if (*pinfo_fd == -1) errExit("shm_open");
  if(ftruncate(*pinfo_fd, processes * sizeof(struct process_info)) == -1) errExit("ftruncate");   
  *pinfo = mmap(NULL, processes * sizeof(struct process_info) ,PROT_READ | PROT_WRITE, MAP_SHARED, *pinfo_fd, 0);
  if(*pinfo == MAP_FAILED) errExit("mmap");
}

void shm_lock_init(struct shm_data* shmp){
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(shmp->lock), &attr);
  pthread_mutexattr_destroy(&attr);
  shmp->counter = 0;
}