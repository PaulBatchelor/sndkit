#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"
#include "dsp/tract.h"

int sk_node_tractxy(sk_core *core);
int sk_node_tractxyv(sk_core *core);
int sk_node_tract(sk_core *core);
sk_tract * sk_node_tractnew(sk_core *core);
int sk_tract_shape(sk_core *core);

static lil_value_t tractxy(lil_t lil,
                           size_t argc,
                           lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tract", argc, 3);

    for (i = 0; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "tract");
    }

    rc = sk_node_tractxy(core);
    SKLIL_ERROR_CHECK(lil, rc, "tractxy didn't work out.");
    return NULL;
}

static lil_value_t tractxyv(lil_t lil,
                           size_t argc,
                           lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tract", argc, 4);

    for (i = 0; i < 4; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "tract");
    }

    rc = sk_node_tractxyv(core);
    SKLIL_ERROR_CHECK(lil, rc, "tractxyv didn't work out.");
    return NULL;
}

static lil_value_t tractnew(lil_t lil,
                            size_t argc,
                            lil_value_t *argv)
{
    sk_core *core;
    int rc;
    sk_tract *tract;

    core = lil_get_data(lil);
    tract = sk_node_tractnew(core);

    rc = sk_core_generic_push(core, tract);
    SKLIL_ERROR_CHECK(lil, rc, "tractnew didn't work out.");
    return NULL;
}

static lil_value_t tract(lil_t lil,
                         size_t argc,
                         lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tract", argc, 3);

    for (i = 1; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "tract");
    }

    rc = sk_node_tract(core);
    SKLIL_ERROR_CHECK(lil, rc, "tract didn't work out.");
    return NULL;
}


static lil_value_t tractshape(lil_t lil,
                              size_t argc,
                              lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tractshape", argc, 2);

    rc = sk_tract_shape(core);
    SKLIL_ERROR_CHECK(lil, rc, "tractshape didn't work out.");
    return NULL;
}

void sklil_load_tract(lil_t lil)
{
    lil_register(lil, "tractxy", tractxy);
    lil_register(lil, "tractxyv", tractxyv);
    lil_register(lil, "tractnew", tractnew);
    lil_register(lil, "tract", tract);
    lil_register(lil, "tractshape", tractshape);
}
