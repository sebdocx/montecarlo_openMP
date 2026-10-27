/* montecarlo.c -- version paralela con OpenMP (version final).
   Autor: Sebastian Villamizar Saavedra - codigo 2230078 (semilla base).
   Uso: ./montecarlo <numero de muestras> [hilos] [semilla]
        (el numero de hilos tambien puede fijarse con OMP_NUM_THREADS)
   Compilar: gcc -O2 -fopenmp montecarlo.c -o montecarlo -lm

   Idea: la secuencia de rand_r con la semilla base se parte en bloques
   consecutivos de BLOQUE muestras. El estado inicial de cada bloque se
   calcula "saltando" hacia adelante en el generador, asi que cada bloque
   consume siempre los mismos numeros, sin importar que hilo lo ejecute.
   Resultado: flujos disjuntos (sin solapamiento) y la misma estimacion
   para cualquier p, identica a la de la version secuencial.

   Nota de portabilidad: rand_r se implementa aqui mismo con el algoritmo
   de glibc (mi_rand_r). Asi el programa da los mismos numeros en Linux,
   WSL o MSYS2/MinGW, donde rand_r no existe o es distinta. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SEMILLA_BASE 2230078u  /* codigo estudiantil: Sebastian Villamizar */
#define BLOQUE 100000LL        /* muestras por bloque */
#define RAND_MAX_GLIBC 2147483647.0

#ifndef PLAN
#define PLAN static            /* -DPLAN=dynamic para otra planificacion */
#endif

/* rand_r de glibc: 3 pasos del LCG s <- 1103515245*s + 12345 (mod 2^32)
   por llamada; el resultado combina 31 bits de los tres estados. */
#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL   /* 2 llamadas (x, y) x 3 pasos */

static int mi_rand_r(unsigned int *seed) {
    unsigned int next = *seed;
    int result;
    next = next * LCG_A + LCG_C;
    result = (int)((next / 65536u) % 2048u);
    next = next * LCG_A + LCG_C;
    result <<= 10;
    result ^= (int)((next / 65536u) % 1024u);
    next = next * LCG_A + LCG_C;
    result <<= 10;
    result ^= (int)((next / 65536u) % 1024u);
    *seed = next;
    return result;
}

/* Avanza el estado s en k pasos del LCG con O(log k) operaciones. */
static unsigned int saltar(unsigned int s, unsigned long long k) {
    unsigned int a = LCG_A, c = LCG_C;  /* transformacion de 2^j pasos */
    unsigned int A = 1u, C = 0u;        /* transformacion acumulada */
    while (k) {
        if (k & 1ULL) {
            A = a * A;
            C = a * C + c;
        }
        c = a * c + c;
        a = a * a;
        k >>= 1;
    }
    return A * s + C;
}

int main(int argc, char *argv[]) {
    long long N = (argc > 1) ? atoll(argv[1]) : 100000000L;
    if (argc > 2) omp_set_num_threads(atoi(argv[2]));
    unsigned int semilla = (argc > 3) ? (unsigned int)strtoul(argv[3], NULL, 10)
                                      : SEMILLA_BASE;

    /* Verifica que el salto coincide con lo que hace mi_rand_r. */
    unsigned int prueba = semilla;
    mi_rand_r(&prueba);
    mi_rand_r(&prueba);
    if (prueba != saltar(semilla, PASOS_POR_MUESTRA)) {
        fprintf(stderr, "error: el salto del LCG no coincide con mi_rand_r\n");
        return 1;
    }

    /* Periodo del estado: 2^32. Con mas de 2^32/6 muestras la secuencia se
       repetiria y los bloques dejarian de ser disjuntos. */
    if ((unsigned long long)N * PASOS_POR_MUESTRA > 4294967296ULL)
        fprintf(stderr, "aviso: N supera el periodo del generador\n");

    long long nbloques = (N + BLOQUE - 1) / BLOQUE;
    long long dentro = 0;

    double t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:dentro) schedule(PLAN)
    for (long long b = 0; b < nbloques; b++) {
        unsigned int estado =
            saltar(semilla, (unsigned long long)b * BLOQUE * PASOS_POR_MUESTRA);
        long long fin = (b + 1) * BLOQUE < N ? (b + 1) * BLOQUE : N;
        for (long long i = b * BLOQUE; i < fin; i++) {
            double x = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            double y = mi_rand_r(&estado) / RAND_MAX_GLIBC;
            if (x * x + y * y <= 1.0)
                dentro++;
        }
    }
    double t = omp_get_wtime() - t0;

    double pi = 4.0 * dentro / (double)N;
    printf("p=%d N=%lld dentro=%lld pi=%.6f |error|=%.2e T=%.4f s\n",
           omp_get_max_threads(), N, dentro, pi, fabs(pi - M_PI), t);
    return 0;
}
