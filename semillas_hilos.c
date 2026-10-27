/* II.2: muestra donde empieza el flujo de cada hilo con p hilos (schedule static)
   y comprueba que saltar() coincide con avanzar el generador paso a paso. */
#include <stdio.h>
#include <stdlib.h>
#include "lcg.h"
#define BLOQUE 100000LL
int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000LL;
    int p = (argc > 2) ? atoi(argv[2]) : 4;
    unsigned int semilla = 2230078u;
    long long nb = (N + BLOQUE - 1) / BLOQUE;
    printf("N=%lld p=%d N/p=%lld semilla base=%u\n", N, p, N / p, semilla);
    printf("%-5s %-11s %-21s %-14s %-14s %s\n", "hilo", "bloques", "muestras que genera",
           "estado inicial", "rand_r secuenc", "ok");
    for (int k = 0; k < p; k++) {
        long long b0 = nb * k / p, b1 = nb * (k + 1) / p - 1;
        unsigned int e = saltar(semilla, (unsigned long long)b0 * BLOQUE * 6ULL);
        unsigned int s = semilla;                       /* avance paso a paso */
        for (long long i = 0; i < b0 * BLOQUE * 2; i++) mi_rand_r(&s);
        printf("%-5d %-11lld %-10lld - %-8lld %-14u %-14u %s\n", k, b0, b0 * BLOQUE, (b1 + 1) * BLOQUE - 1,
               e, s, e == s ? "si" : "NO");
    }
    return 0;
}
