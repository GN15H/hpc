#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    unsigned long long n = atoll(argv[1]);
    int num_threads = atoi(argv[2]);
    if (n == 0 || num_threads <= 0) return 1;

    omp_set_num_threads(num_threads);

    double t0 = omp_get_wtime();

    unsigned long long hits = 0;
    const double factor = 1.0 / RAND_MAX;

    #pragma omp parallel
    {
        unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();

        #pragma omp for schedule(static) reduction(+:hits)
        for (unsigned long long i = 0; i < n; ++i) {
            double x = rand_r(&seed) * factor;
            double y = rand_r(&seed) * factor;
            hits += (x*x + y*y < 1.0);
        }
    }

    double elapsed = omp_get_wtime() - t0;
    double pi_approx = 4.0 * (double)hits / (double)n;

    printf("%.6f",elapsed);
    // printf("approx: %f", pi_approx);

    return 0;
}

