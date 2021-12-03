#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_mul(sk_core *core);
int sk_node_div(sk_core *core);
int sk_node_add(sk_core *core);
int sk_node_sub(sk_core *core);

static lil_value_t add(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "add", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "add");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "add");

    rc = sk_node_add(core);
    SKLIL_ERROR_CHECK(lil, rc, "add did not work.");
    return NULL;
}

static lil_value_t mul(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "mul", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "mul");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "mul");

    rc = sk_node_mul(core);
    SKLIL_ERROR_CHECK(lil, rc, "mul did not work.");
    return NULL;
}

void sklil_load_arith(lil_t lil)
{
    lil_register(lil, "add", add);
    lil_register(lil, "mul", mul);
}
