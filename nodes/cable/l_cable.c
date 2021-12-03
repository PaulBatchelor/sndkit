#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_cabnew(sk_core *core);

static lil_value_t cabnew(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "cabnew", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "cabnew");

    rc = sk_node_cabnew(core);
    SKLIL_ERROR_CHECK(lil, rc, "cabnew didn't work out.");
    return NULL;
}

void sklil_load_cable(lil_t lil)
{
    lil_register(lil, "cabnew", cabnew);
}
