#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "peakeq.h"

static lil_value_t peakeq(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "peakeq", argc, 1);

    for (i = 0; i < 4; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "peakeq");
    }

    rc = sk_node_peakeq(core);
    SKLIL_ERROR_CHECK(lil, rc, "peakeq didn't work out.");
    return NULL;
}

void sklil_load_peakeq(lil_t lil)
{
    lil_register(lil, "peakeq", peakeq);
}
