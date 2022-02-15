#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_cabnew(sk_core *core);
int sk_node_mix(sk_core *core);
int sk_node_zero(sk_core *core);
int sk_node_prev(sk_core *core);
int sk_node_cabclr(sk_core *core);

static lil_value_t cabnew(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "cabnew", argc, 1);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "cabnew");

    rc = sk_node_cabnew(core);
    SKLIL_ERROR_CHECK(lil, rc, "cabnew didn't work out.");
    return NULL;
}

static lil_value_t mix(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int c;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "mix", argc, 3);

    for (c = 0; c < 3; c++) {
        rc = sklil_param(core, argv[c]);
        SKLIL_PARAM_CHECK(lil, rc, "mix");
    }

    rc = sk_node_mix(core);
    SKLIL_ERROR_CHECK(lil, rc, "mix didn't work out.");
    return NULL;
}

static lil_value_t zero(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);
    rc = sk_node_zero(core);
    SKLIL_ERROR_CHECK(lil, rc, "zero didn't work out.");
    return NULL;
}

static lil_value_t prev(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);
    rc = sk_node_prev(core);
    SKLIL_ERROR_CHECK(lil, rc, "prev didn't work out.");
    return NULL;
}

static lil_value_t cabclr(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    SKLIL_ARITY_CHECK(lil, "cabclr", argc, 1);

    core = lil_get_data(lil);
    rc = sk_node_cabclr(core);
    SKLIL_ERROR_CHECK(lil, rc, "cabclr didn't work out.");
    return NULL;
}

void sklil_load_cable(lil_t lil)
{
    lil_register(lil, "cabnew", cabnew);
    lil_register(lil, "mix", mix);
    lil_register(lil, "zero", zero);
    lil_register(lil, "prev", prev);
    lil_register(lil, "cabclr", cabclr);
}
