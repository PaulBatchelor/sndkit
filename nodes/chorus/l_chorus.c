#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "chorus.h"

static lil_value_t chorus(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "chorus", argc, 6);

    for (i = 0; i < 6; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "chorus");
    }

    rc = sk_node_chorus(core);
    SKLIL_ERROR_CHECK(lil, rc, "chorus didn't work out.");
    return NULL;
}

void sklil_load_chorus(lil_t lil)
{
    lil_register(lil, "chorus", chorus);
}
