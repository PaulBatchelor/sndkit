#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_blsaw(sk_core *core);
int sk_node_blsquare(sk_core *core);
int sk_node_bltri(sk_core *core);

static lil_value_t blsaw(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "blsaw", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "blsaw");

    rc = sk_node_blsaw(core);
    SKLIL_ERROR_CHECK(lil, rc, "blsaw didn't work out.");
    return NULL;
}

static lil_value_t blsquare(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "blsquare", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "blsquare");

    rc = sk_node_blsquare(core);
    SKLIL_ERROR_CHECK(lil, rc, "blsquare didn't work out.");
    return NULL;
}

static lil_value_t bltri(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "bltri", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "bltri");

    rc = sk_node_bltri(core);
    SKLIL_ERROR_CHECK(lil, rc, "bltri didn't work out.");
    return NULL;
}

void sklil_load_blep(lil_t lil)
{
    lil_register(lil, "blsaw", blsaw);
    lil_register(lil, "blsquare", blsquare);
    lil_register(lil, "bltri", bltri);
}
