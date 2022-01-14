#ifndef SKLIL_H
#define SKLIL_H
#define SKLIL_ARITY_CHECK(LIL, NODE, ARGC, NARGS) \
if (ARGC < NARGS) { \
    lil_set_error(LIL, NODE ": not enough args."); \
    return NULL; \
}

#define SKLIL_ERROR_CHECK(lil, rc, msg) \
if (rc) { \
    lil_set_error(lil, msg); \
    lil_set_errcode(rc); \
    return NULL; \
}

#define SKLIL_PARAM_CHECK(lil, rc, node) \
if (rc) { \
    lil_set_error(lil, node " parameter error."); \
    return NULL; \
}

int sklil_param(sk_core *core, lil_value_t arg);

void sklil_nodes(lil_t lil);
void sklil_loader(lil_t lil);
void sklil_loader_withextra(lil_t lil);
void sklil_clean(lil_t lil);
int sklil_main(int argc, char *argv[]);
void lil_set_errcode(int err);
#endif
