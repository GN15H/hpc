#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>

//NOTA AL MARGE: rand es una malparida puta mierda

struct thread_info {   
    pthread_t thread_id;
    long long unsigned int crosses;
    long long unsigned int tries;
    double x_factor;
    double theta_factor;
    double floor;
    double needle;
    char padding[8];
};

void* approx_pi(void* data);

int main(int argc, char** argv){
    if(argc < 3) return 1;
    if(atoi(argv[1]) == 0 || atoi(argv[2]) == 0) return 1;
    srand(time(NULL));
    struct timespec start, end;

    const double floor = 1.0;
    const double needle = 0.5;
    const long long unsigned int n = atoi(argv[1]);
    unsigned int threads = atoi(argv[2]);
    long long unsigned int crosses = 0;
    long long unsigned int thread_tries = n / threads;
    double x, theta; 
    double x_factor = floor / RAND_MAX;
    double theta_factor = M_PI / RAND_MAX;
    struct thread_info tinfo[threads];
    void* res;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(unsigned int i=0; i<threads; ++i){
        tinfo[i].floor = floor;
        tinfo[i].crosses = 0;
        tinfo[i].needle = needle;
        tinfo[i].tries = thread_tries;
        tinfo[i].theta_factor = theta_factor;
        tinfo[i].x_factor = x_factor;
        pthread_create(&tinfo[i].thread_id, NULL, approx_pi, (void*) &tinfo[i]);
    }

    for(unsigned int i=0; i<threads; ++i) pthread_join(tinfo[i].thread_id, &res);

    for(unsigned int i=0; i<threads; ++i) crosses+=tinfo[i].crosses;

    const double pi_approx = (needle*((double)n))/(((double)crosses)*floor);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,", elapsed);
    // printf("%.10lf", pi_approx);

    return 0;
}

void* approx_pi(void* data){
    struct thread_info* tinfo = data;
    double x, theta;
    unsigned int seed = time(NULL) ^ (uintptr_t)pthread_self(); // unique seed
    for(long long unsigned int i=0; i<tinfo->tries; ++i){
        x = rand_r(&seed) * tinfo->x_factor;
        theta = rand_r(&seed) * tinfo->theta_factor;
        if(x-(cos(theta)*(tinfo->needle/2)) < 0.0 || x+(cos(theta)*(tinfo->needle/2)) > tinfo->floor)
            ++(tinfo->crosses);
    }
    return NULL;
}