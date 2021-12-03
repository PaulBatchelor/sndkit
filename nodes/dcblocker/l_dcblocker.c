#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_dcblocker(sk_core *core);

static lil_value_t dcblocker(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "dcblocker", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "dcblocker");

    rc = sk_node_dcblocker(core);

    SKLIL_ERROR_CHECK(lil, rc, "dcblocker didn't work out.");
    return NULL;
}

void sklil_load_dcblocker(lil_t lil)
{
    lil_register(lil, "dcblocker", dcblocker);
}
