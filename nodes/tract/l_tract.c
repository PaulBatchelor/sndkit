#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_tractxy(sk_core *core);
int sk_node_tract(sk_core *core);

static lil_value_t tractxy(lil_t lil,
                           size_t argc,
                           lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tract", argc, 3);

    for (i = 0; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "tract");
    }

    rc = sk_node_tractxy(core);
    SKLIL_ERROR_CHECK(lil, rc, "tractxy didn't work out.");
    return NULL;
}

void sklil_load_tract(lil_t lil)
{
    lil_register(lil, "tractxy", tractxy);
}
