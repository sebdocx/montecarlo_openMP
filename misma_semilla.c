/* II.3 (problema 2): cada hilo tiene su generador PERO todos con la misma semilla. */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lcg.h"
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    long long dentro = 0;
    double t0 = omp_get_wtime();
    #pragma omp parallel reduction(+:dentro)
    {
        unsigned int estado = 2230078u;          /* MISMA semilla en todos */
        int p = omp_get_num_threads(), k = omp_get_thread_num();
        long long ini = N * k / p, fin = N * (k + 1) / p;
        for (long long i = ini; i < fin; i++) {
            double x = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            double y = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            if (x * x + y * y <= 1.0) dentro++;
        }
    }
    double t = omp_get_wtime() - t0, pi = 4.0 * dentro / (double)N;
    printf("misma_semilla p=%d dentro=%lld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
