#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
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

    rc = sk_core_hold(core);

    SKLIL_ERROR_CHECK(lil, rc, "unhold didn't work out.");

    return NULL;
}

void sklil_load_buffer(lil_t lil)
{
    lil_register(lil, "hold", hold);
    lil_register(lil, "unhold", unhold);
}
