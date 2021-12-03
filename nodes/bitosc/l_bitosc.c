#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_bitosc(sk_core *core);

static lil_value_t bitosc(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "bitosc", argc, 3);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "bitosc");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "bitosc");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "bitosc");

    rc = sk_node_bitosc(core);
    SKLIL_ERROR_CHECK(lil, rc, "bitosc didn't work out.");
    return NULL;
}

void sklil_load_bitosc(lil_t lil)
{
    lil_register(lil, "bitosc", bitosc);
}
