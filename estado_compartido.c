/* II.3 (problema 1): TODOS los hilos usan UN estado global (como rand()). Condicion de carrera. */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lcg.h"
static unsigned int estado_global = 2230078u;   /* compartido */
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    long long dentro = 0;
    double t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:dentro)
    for (long long i = 0; i < N; i++) {
        double x = mi_rand_r(&estado_global) / RAND_MAX_GLIBC;
        double y = mi_rand_r(&estado_global) / RAND_MAX_GLIBC;
        if (x * x + y * y <= 1.0) dentro++;
    }
    double t = omp_get_wtime() - t0, pi = 4.0 * dentro / (double)N;
    printf("estado_compartido p=%d dentro=%lld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
