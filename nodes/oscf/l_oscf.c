#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_oscf(sk_core *core);
int sk_node_oscfext(sk_core *core);

static lil_value_t oscf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "oscf", argc, 3);

    /* skip param 0 containing table */
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "oscf");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "oscf");

    rc = sk_node_oscf(core);

    SKLIL_ERROR_CHECK(lil, rc, "There was a problem with oscf.")
    return NULL;
}

static lil_value_t oscfext(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "oscfext", argc, 2);

    /* skip param 0 containing table */
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "oscfext");

    rc = sk_node_oscfext(core);

    SKLIL_ERROR_CHECK(lil, rc, "There was a problem with oscfext.")
    return NULL;
}

void sklil_load_oscf(lil_t lil)
{
    lil_register(lil, "oscf", oscf);
    lil_register(lil, "oscfext", oscfext);
}
