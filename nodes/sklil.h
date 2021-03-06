#ifndef SKLIL_H
#define SKLIL_H
#define SKLIL_ARITY_CHECK(LIL, NODE, ARGC, NARGS) \
if (ARGC < NARGS) { \
    lil_set_error(LIL, NODE ": not enough args."); \
    return NULL; \
}

void sklil_param(sk_core *core, lil_value_t arg);
#endif
