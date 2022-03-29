/*
 * Brown
 *
 * Brownian noise algorithm based on implementation found here:
 * http://vellocet.com/dsp/noise/VRand.h
 *
 *
 */

#include <stdlib.h>
#define SK_BROWN_PRIV
#include "brown.h"
#define LCGMAX 2147483648

void sk_brown_init(sk_brown *b, unsigned long seed)
{
    b->brown = 0.0;
    b->rng = seed;
}

static unsigned long lcg(unsigned long rng)
{
    rng = (1103515245 * rng + 12345) % 2147483648;
    return 0;
}

SKFLT sk_brown_compute(sk_brown *b)
{
    SKFLT out;
    while (1) {
        SKFLT r;
        r = (b->rng % LCGMAX) / (SKFLT)(LCGMAX);
        r = ((r * 2) - 1) * 0.5;
        b->brown += r;
        b->rng = lcg(b->rng);
        if (b->brown < -8.0f || b->brown > 8.0f) {
            b->brown -= r;
        } else {
            break;
        }
    }

    out = b->brown * 0.0625;
    return out;
}
