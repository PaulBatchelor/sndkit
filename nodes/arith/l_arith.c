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

static lil_value_t sub(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "sub", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "sub");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "sub");

    rc = sk_node_sub(core);
    SKLIL_ERROR_CHECK(lil, rc, "sub did not work.");
    return NULL;
}

/* div is a stdlib function, hence divi*/
static lil_value_t divi(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "div", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "div");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "div");

    rc = sk_node_div(core);
    SKLIL_ERROR_CHECK(lil, rc, "div did not work.");
    return NULL;
}

void sklil_load_arith(lil_t lil)
{
    lil_register(lil, "add", add);
    lil_register(lil, "mul", mul);
    lil_register(lil, "sub", sub);
    lil_register(lil, "div", divi);
}
