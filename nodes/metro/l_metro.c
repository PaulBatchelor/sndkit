#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_metro(sk_core *core);

static lil_value_t metro(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "metro", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "metro");

    rc = sk_node_metro(core);
    SKLIL_ERROR_CHECK(lil, rc, "metro didn't work out.");
    return NULL;
}

void sklil_load_metro(lil_t lil)
{
    lil_register(lil, "metro", metro);
}
