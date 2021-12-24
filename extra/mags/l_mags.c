#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_mags(sk_core *core);

static lil_value_t mags(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "mags", argc, 1);

    rc = sk_mags(core);
    SKLIL_ERROR_CHECK(lil, rc, "mags didn't work out.");

    return NULL;
}

void sklil_load_mags(lil_t lil)
{
    lil_register(lil, "mags", mags);
}
