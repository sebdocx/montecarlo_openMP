/* montecarlo_ref.c -- programa del enunciado (version secuencial), con la
   semilla 2230078. rand_r se reemplaza por mi_rand_r (algoritmo de glibc)
   para que compile igual en Linux, WSL y MSYS2. */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "pruebas/lcg.h"
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    unsigned int estado = 2230078u; /* semilla del generador */
    long long dentro = 0;
    double t0 = omp_get_wtime();
    for (long long i = 0; i < N; i++) {
        double x = mi_rand_r(&estado) / RAND_MAX_GLIBC;
        double y = mi_rand_r(&estado) / RAND_MAX_GLIBC;
        if (x * x + y * y <= 1.0)
            dentro++;
    }
    double t = omp_get_wtime() - t0;
    printf("secuencial N=%lld dentro=%lld pi=%.6f |error|=%.2e T=%.4f s\n",
           N, dentro, 4.0 * dentro / (double)N, fabs(4.0 * dentro / (double)N - M_PI), t);
    return 0;
}
