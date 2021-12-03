#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_biramp(sk_core *core);
int sk_node_flipper(sk_core *core);

static lil_value_t biramp(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "biramp", argc, 2);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "biramp");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "biramp");

    rc = sk_node_biramp(core);
    SKLIL_ERROR_CHECK(lil, rc, "biramp didn't work out.");
    return NULL;
}

static lil_value_t flipper(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "flipper", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "flipper");

    rc = sk_node_flipper(core);
    SKLIL_ERROR_CHECK(lil, rc, "flipper didn't work out.");
    return NULL;
}

void sklil_load_biramp(lil_t lil)
{
    lil_register(lil, "biramp", biramp);
    lil_register(lil, "flipper", flipper);
}
