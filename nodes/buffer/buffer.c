#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

static lil_value_t hold(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    rc = sk_core_hold(core);

    SKLIL_ERROR_CHECK(lil, rc, "hold didn't work out.");

    return NULL;
}

static lil_value_t unhold(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    rc = sk_core_unhold(core);

    SKLIL_ERROR_CHECK(lil, rc, "unhold didn't work out.");

    return NULL;
}

static lil_value_t lil_dup(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    rc = sk_core_dup(core);

    SKLIL_ERROR_CHECK(lil, rc, "dup didn't work out.");

    return NULL;
}

static lil_value_t drop(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    rc = sk_core_drop(core);

    SKLIL_ERROR_CHECK(lil, rc, "drop didn't work out.");

    return NULL;
}

static lil_value_t swap(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    rc = sk_core_swap(core);

    SKLIL_ERROR_CHECK(lil, rc, "swap didn't work out.");

    return NULL;
}

void sklil_load_buffer(lil_t lil)
{
    lil_register(lil, "hold", hold);
    lil_register(lil, "unhold", unhold);
    lil_register(lil, "dup", lil_dup);
    lil_register(lil, "drop", drop);
    lil_register(lil, "swap", swap);
}
