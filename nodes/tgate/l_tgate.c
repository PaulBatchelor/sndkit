#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_tgate(sk_core *core);

static lil_value_t tgate(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int n;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tgate", argc, 2);

    for (n = 0; n < 2; n++) {
        rc = sklil_param(core, argv[n]);
        SKLIL_PARAM_CHECK(lil, rc, "tgate");
    }

    rc = sk_node_tgate(core);
    SKLIL_ERROR_CHECK(lil, rc, "tgate didn't work out.");
    return NULL;
}

void sklil_load_tgate(lil_t lil)
{
    lil_register(lil, "tgate", tgate);
}
