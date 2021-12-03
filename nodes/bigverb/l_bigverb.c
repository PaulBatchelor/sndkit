#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_bigverb(sk_core *core);

static lil_value_t bigverb(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "bigverb", argc, 4);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "bigverb");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "bigverb");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "bigverb");
    rc = sklil_param(core, argv[3]);
    SKLIL_PARAM_CHECK(lil, rc, "bigverb");

    rc = sk_node_bigverb(core);
    SKLIL_ERROR_CHECK(lil, rc, "bigverb didn't work out.");
    return NULL;
}

void sklil_load_bigverb(lil_t lil)
{
    lil_register(lil, "bigverb", bigverb);
}
