#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_rline(sk_core *core);
int sk_node_jitseg(sk_core *core);

static lil_value_t rline(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "rline", argc, 3);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "rline");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "rline");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "rline");

    rc = sk_node_rline(core);
    SKLIL_ERROR_CHECK(lil, rc, "rline didn't work out.");
    return NULL;
}

static lil_value_t jitseg(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "jitseg", argc, 5);

    for (i = 0; i < 5; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "jitseg");
    }

    rc = sk_node_jitseg(core);
    SKLIL_ERROR_CHECK(lil, rc, "jitseg didn't work out.");
    return NULL;
}

void sklil_load_rline(lil_t lil)
{
    lil_register(lil, "rline", rline);
    lil_register(lil, "jitseg", jitseg);
}
