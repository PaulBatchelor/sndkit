#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "oscf.h"

static lil_value_t oscf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "oscf", argc, 3);

    /* skip param 0 containing table */
    sklil_param(core, argv[1]);
    sklil_param(core, argv[2]);

    rc = sk_node_oscf(core);

    SKLIL_ERROR_CHECK(lil, rc, "There was a problem with oscf.")
    return NULL;
}

void sklil_load_oscf(lil_t lil)
{
    lil_register(lil, "oscf", oscf);
}
