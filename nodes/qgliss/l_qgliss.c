#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_qgliss(sk_core *core);

static lil_value_t l_qgliss(lil_t lil,
                            size_t argc,
                            lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "qgliss", argc, 4);

    for (i = 1; i < 3; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "qgliss");
    }

    rc = sk_node_qgliss(core);

    SKLIL_ERROR_CHECK(lil, rc, "qgliss didn't work out.");

    return NULL;
}

void sklil_load_qgliss(lil_t lil)
{
    lil_register(lil, "qgliss", l_qgliss);
}
