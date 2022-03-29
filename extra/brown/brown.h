#ifndef SK_BROWN_H
#define SK_BROWN_H

#ifndef SKFLT
#define SKFLT float
#endif

typedef struct sk_brown sk_brown;

#ifdef SK_BROWN_PRIV
struct sk_brown {
    SKFLT brown;
    unsigned long rng;
};
#endif

void sk_brown_init(sk_brown *brown, unsigned long seed);
SKFLT sk_brown_compute(sk_brown *brown);
#endif
