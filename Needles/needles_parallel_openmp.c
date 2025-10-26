#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    const long long unsigned int n = atoll(argv[1]);
    const int num_threads = atoi(argv[2]);

    const double floor = 1.0;
    const double needle = 0.5;
    const double half_needle = needle / 2.0;
    const double inv_rand = 1.0 / RAND_MAX;

    long long unsigned int total_crosses = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    omp_set_num_threads(num_threads);

    #pragma omp parallel reduction(+:total_crosses)
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num(); 

        #pragma omp for
        for (long long unsigned int i = 0; i < n; i++) {
            double x = rand_r(&seed) * inv_rand;
            double theta = rand_r(&seed) * (M_PI / RAND_MAX);

            double c = cos(theta) * half_needle;
            total_crosses += (x < c) + (x > (floor - c));
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double pi_estimate = (needle * (double)n) / ((double)total_crosses * floor);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("%.6f", elapsed);
    // printf("π ≈ %.10f\n", pi_estimate);

    return 0;
}

