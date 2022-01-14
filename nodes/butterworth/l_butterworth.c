#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_butlp(sk_core *core);
int sk_node_buthp(sk_core *core);

static lil_value_t butlp(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "butlp", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "butlp");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "butlp");

    rc = sk_node_butlp(core);
    SKLIL_ERROR_CHECK(lil, rc, "butlp didn't work out.");
    return NULL;
}

static lil_value_t buthp(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "buthp", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "buthp");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "buthp");

    rc = sk_node_buthp(core);
    SKLIL_ERROR_CHECK(lil, rc, "buthp didn't work out.");
    return NULL;
}

void sklil_load_butterworth(lil_t lil)
{
    lil_register(lil, "butlp", butlp);
    lil_register(lil, "buthp", buthp);
}
