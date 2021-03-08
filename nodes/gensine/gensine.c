#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "dsp/gen.h"

static lil_value_t gensine(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    sk_table *tab;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "gensine", argc, 1);

    rc = sk_core_table_pop(core, &tab);

    if (rc) {
        /* TODO: error handling */
        printf("uh oh\n");
    }

    sk_gen_sine(sk_table_data(tab), sk_table_size(tab));

    rc = sk_core_table_push(core, tab);

    if (rc) {
        /* TODO: error handling */
    }

    return NULL;
}

void sklil_load_gensine(lil_t lil)
{
    lil_register(lil, "gensine", gensine);
}
