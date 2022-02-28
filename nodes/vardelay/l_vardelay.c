#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_vardelay(sk_core *core);
int sk_node_clkdel(sk_core *core);

static lil_value_t vardelay(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "vardelay", argc, 4);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "vardelay");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "vardelay");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "vardelay");
    rc = sklil_param(core, argv[3]);
    SKLIL_PARAM_CHECK(lil, rc, "vardelay");

    rc = sk_node_vardelay(core);
    SKLIL_ERROR_CHECK(lil, rc, "vardelay didn't work out.");
    return NULL;
}

static lil_value_t clkdel(lil_t lil,
                          size_t argc,
                          lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "clkdel", argc, 4);

    for (i = 0; i < 4; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "clkdel");
    }

    rc = sk_node_clkdel(core);

    SKLIL_ERROR_CHECK(lil, rc, "glotwave didn't work out.");

    return NULL;
}

void sklil_load_vardelay(lil_t lil)
{
    lil_register(lil, "vardelay", vardelay);
    lil_register(lil, "clkdel", clkdel);
}
