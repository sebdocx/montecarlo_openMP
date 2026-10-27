#ifndef LCG_H
#define LCG_H
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define RAND_MAX_GLIBC 2147483647.0
#define LCG_A 1103515245u
#define LCG_C 12345u
#define PASOS_POR_MUESTRA 6ULL   /* 2 llamadas (x, y) x 3 pasos */

static inline int mi_rand_r(unsigned int *seed) {
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
static inline unsigned int saltar(unsigned int s, unsigned long long k) {
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

#endif
