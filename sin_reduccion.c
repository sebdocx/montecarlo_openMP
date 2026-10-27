/* II.4: acumulador compartido SIN reduction -> condicion de carrera (data race). */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lcg.h"
#define BLOQUE 100000LL
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    long long nb = (N + BLOQUE - 1) / BLOQUE, dentro = 0;
    double t0 = omp_get_wtime();
    #pragma omp parallel for schedule(static)      /* <- sin reduction */
    for (long long b = 0; b < nb; b++) {
        unsigned int estado = saltar(2230078u, (unsigned long long)b * BLOQUE * 6ULL);
        long long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;
        for (long long i = b * BLOQUE; i < fin; i++) {
            double x = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            double y = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            if (x * x + y * y <= 1.0) dentro++;    /* dentro es compartida */
        }
    }
    double t = omp_get_wtime() - t0, pi = 4.0 * dentro / (double)N;
    printf("sin_reduccion p=%d dentro=%lld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
