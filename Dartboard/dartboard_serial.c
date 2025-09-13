#include<stdlib.h>
#include<time.h>
#include<stdio.h>

int main(int argc, char** argv){
    if(argc < 2) return 1;
    srand(time(NULL));
    struct timespec start, end;
    unsigned long long int n = atoi(argv[1]);
    unsigned long long int hits = 0;
    double x, y;
    const double factor = 1.0 / RAND_MAX;
    if(n == 0) return 1;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0; i<n; ++i){
        x = rand()*factor;
        y = rand()*factor;
        if(x*x + y*y < 1.0)
            ++hits;
    }
    const double pi_approx = (4.0 * hits) / n; 
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,", elapsed);

    return 0;
}