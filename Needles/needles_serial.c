#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
    if(argc < 2) return 1;
    if(atoi(argv[1]) == 0) return 1;
    srand(time(NULL));
    struct timespec start, end;

    const double floor = 1.0;
    const double needle = 0.5;
    const long long unsigned int n = atoi(argv[1]);
    long long unsigned int crosses = 0;
    double x, theta; 
    double x_factor = floor / RAND_MAX;
    double theta_factor = M_PI / RAND_MAX;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(long long unsigned int i=0; i<n; ++i){
        x = rand() * x_factor;
        theta = rand() * theta_factor;
        if(x-(cos(theta)*(needle/2)) < 0.0 || x+(cos(theta)*(needle/2)) > floor)
            ++crosses;
    }
    const double pi_approx = (needle*((double)n))/(((double)crosses)*floor);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f,", elapsed);

    // printf("%.10lf", pi_approx);



    return 0;
}