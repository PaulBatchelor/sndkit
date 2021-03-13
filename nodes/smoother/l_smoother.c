#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "smoother.h"

static lil_value_t smoother(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "smoother", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "smoother");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "smoother");

    rc = sk_node_smoother(core);
    SKLIL_ERROR_CHECK(lil, rc, "smoother didn't work out.");
    return NULL;
}

void sklil_load_smoother(lil_t lil)
{
    lil_register(lil, "smoother", smoother);
}
