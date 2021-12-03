#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge/graforge.h"
#include "core.h"
#include "sknodes.h"


#define ARITY_CHECK(LIL, NODE, ARGC, NARGS) \
if (ARGC < NARGS) { \
    lil_set_error(lil, NODE ": not enough args."); \
    return NULL; \
}

const char * patch=
"sine 350 0.2;"
"sine [add [sine 0.1 100] 440] 0.2;"
"add zz zz;"
"wavout zz test.wav;"
;

static void param(sk_core *core, lil_value_t arg)
{
    const char *sarg;
    sarg = lil_to_string(arg);

    if (!strcmp(sarg, "zz") || strlen(sarg) == 0) {
        return;
    }

    sk_core_constant(core, lil_to_double(arg));
}

static lil_value_t sine(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    ARITY_CHECK(lil, "sine", argc, 2);

    param(core, argv[0]);
    param(core, argv[1]);

    sk_node_sine(core);
    return NULL;
}

static lil_value_t add(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    param(core, argv[0]);
    param(core, argv[1]);

    sk_node_add(core);
    return NULL;
}

static lil_value_t wavout(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;

    core = lil_get_data(lil);

    param(core, argv[0]);

    sk_node_wavout(core, lil_to_string(argv[1]));
    return NULL;
}

int main(int argc, char *argv[])
{
    lil_t lil;
    lil_value_t val;
    sk_core *core;
    const char *err_msg;
    unsigned int nblocks, n;
    size_t pos;

    lil = lil_new();
    core = sk_core_new(44100);
    lil_set_data(lil, core);

    lil_register(lil, "sine", sine);
    lil_register(lil, "wavout", wavout);
    lil_register(lil, "add", add);
    val = lil_parse(lil, patch, strlen(patch), 0);
    lil_free_value(val);

    if (lil_error(lil, &err_msg, &pos)) {
        fprintf(stderr, "lil: error at %i: %s\n", (int)pos, err_msg);
        goto clean;
    }

    nblocks = sk_core_seconds_to_blocks(core, 10);

    for (n = 0; n < nblocks; n++) {
        sk_core_compute(core);
    }

    clean:
    lil_free(lil);
    sk_core_del(core);
    return 0;
}
