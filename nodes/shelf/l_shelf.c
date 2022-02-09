#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_highshelf(sk_core *core);
int sk_node_lowshelf(sk_core *core);

static lil_value_t highshelf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "highshelf", argc, 4);

    for (i = 0; i < 4; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "highshelf");
    }

    rc = sk_node_highshelf(core);
    SKLIL_ERROR_CHECK(lil, rc, "highshelf didn't work out.");
    return NULL;
}

static lil_value_t lowshelf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "lowshelf", argc, 4);

    for (i = 0; i < 4; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "lowshelf");
    }

    rc = sk_node_lowshelf(core);
    SKLIL_ERROR_CHECK(lil, rc, "lowshelf didn't work out.");
    return NULL;
}

void sklil_load_shelf(lil_t lil)
{
    lil_register(lil, "highshelf", highshelf);
    lil_register(lil, "lowshelf", lowshelf);
}
