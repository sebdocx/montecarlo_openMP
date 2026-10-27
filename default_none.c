/* II.4: default(none) OBLIGA al compilador a nombrar las variables compartidas.
   Este archivo NO compila a proposito: los errores listan cuales son. */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lcg.h"
#define BLOQUE 100000LL
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    unsigned int semilla = 2230078u;
    long long nbloques = (N + BLOQUE - 1) / BLOQUE, dentro = 0;
    #pragma omp parallel for default(none) reduction(+:dentro)
    for (long long b = 0; b < nbloques; b++) {
        unsigned int estado = saltar(semilla, (unsigned long long)b * BLOQUE * 6ULL);
        long long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;
        for (long long i = b * BLOQUE; i < fin; i++) {
            double x = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            double y = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            if (x * x + y * y <= 1.0) dentro++;
        }
    }
    printf("%lld\n", dentro);
    return 0;
}
