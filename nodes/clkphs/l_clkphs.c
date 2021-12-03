#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_clkphs(sk_core *core);

static lil_value_t clkphs(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "clkphs", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "clkphs");

    rc = sk_node_clkphs(core);
    SKLIL_ERROR_CHECK(lil, rc, "clkphs didn't work out.");
    return NULL;
}

void sklil_load_clkphs(lil_t lil)
{
    lil_register(lil, "clkphs", clkphs);
}
