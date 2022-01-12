#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_trand(sk_core *core);

static lil_value_t trand(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "trand", argc, 3);

    for (i = 0; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "trand");
    }

    rc = sk_node_trand(core);
    SKLIL_ERROR_CHECK(lil, rc, "trand didn't work out.");
    return NULL;
}

void sklil_load_trand(lil_t lil)
{
    lil_register(lil, "trand", trand);
}
