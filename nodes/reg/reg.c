#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

static lil_value_t regset(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int pos;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "regset", argc, 2);

    /* ignore first argument */

    pos = lil_to_integer(argv[1]);

    rc = sk_core_regset(core, pos);

    SKLIL_ERROR_CHECK(lil, rc, "regset: regset failed.");

    return NULL;
}

static lil_value_t regget(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int pos;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "regget", argc, 1);

    pos = lil_to_integer(argv[0]);

    rc = sk_core_regget(core, pos);

    SKLIL_ERROR_CHECK(lil, rc, "regget: regget failed.");

    return NULL;
}

void sklil_load_reg(lil_t lil)
{
    lil_register(lil, "regset", regset);
    lil_register(lil, "regget", regget);
}
