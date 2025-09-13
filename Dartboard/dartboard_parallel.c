#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<pthread.h>
#include<stdint.h>

struct thread_info {   
    pthread_t thread_id;
    long long int hits;
    long long int tries;
    double factor;
    char cond[32];
    //nota notosa poner aca basura ke ocupe el resto de 64 bytes para ke el cache coja este pedazo completo
    //y los diferentes cores no tengan ke hacer sincronización de datos
};

void* approx_pi(void* data);

int main(int argc, char** argv){
    if(argc < 3) return 1;
    if(atoi(argv[1]) == 0 || atoi(argv[2]) == 0) return 1;
    srand(time(NULL));

    struct timespec start, end;
    int s;
    double x, y;
    void *res;

    const unsigned long long int n = atoi(argv[1]);
    const int threads = atoi(argv[2]);
    struct thread_info tinfo[threads];
    unsigned long long int hits = 0;
    const double factor = 1.0 / RAND_MAX;
    const unsigned long long int thread_hits = n/threads;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0; i<threads; i++){
        tinfo[i].factor = factor;
        tinfo[i].tries = thread_hits;
        tinfo[i].hits = 0;
        pthread_create(&tinfo[i].thread_id,NULL, approx_pi, (void*) &tinfo[i]);
    }

    for(int i=0; i<threads; i++){
        pthread_join(tinfo[i].thread_id, &res);
    }

    for(int i=0; i<threads; i++)
        hits += tinfo[i].hits;

    const double pi_approx = (4.0 * hits) / (threads*thread_hits); 
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,\n", elapsed);

    return 0;
}


void* approx_pi(void* data){
    struct thread_info* info = data; 
    double x,y;
    unsigned int seed = time(NULL) ^ (uintptr_t)pthread_self(); // unique seed
    for(int i=0; i<info->tries; ++i){
        x = rand_r(&seed)*info->factor;
        y = rand_r(&seed)*info->factor;
        if(x*x + y*y < 1.0)
            ++(info->hits);
    }
    return NULL;
}