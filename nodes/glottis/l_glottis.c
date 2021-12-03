#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_glottis(sk_core *core);

static lil_value_t l_glottis(lil_t lil, size_t argc, lil_value_t *argv)
{
    int rc;
    int i;
    sk_core *core;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "glottis", argc, 2);

    for (i = 0; i < 2; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "glottis");
    }

    rc = sk_node_glottis(core);
    SKLIL_ERROR_CHECK(lil, rc, "glottis didn't work out.");
    return NULL;
}

void sklil_load_glottis(lil_t lil)
{
    lil_register(lil, "glottis", l_glottis);
}
