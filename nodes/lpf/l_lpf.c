#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_lpf(sk_core *core);

static lil_value_t lpf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "lpf", argc, 3);

    for (i = 0; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "lpf");
    }

    rc = sk_node_lpf(core);
    SKLIL_ERROR_CHECK(lil, rc, "lpf didn't work out.");
    return NULL;
}

void sklil_load_lpf(lil_t lil)
{
    lil_register(lil, "lpf", lpf);
}
