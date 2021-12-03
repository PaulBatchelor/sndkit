#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_scale(sk_core *core);
int sk_node_biscale(sk_core *core);

static lil_value_t biscale(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "biscale", argc, 3);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);
    sklil_param(core, argv[2]);

    sk_node_biscale(core);
    return NULL;
}

static lil_value_t scale(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "scale", argc, 3);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);
    sklil_param(core, argv[2]);

    sk_node_scale(core);
    return NULL;
}

void sklil_load_scale(lil_t lil)
{
    lil_register(lil, "biscale", biscale);
    lil_register(lil, "scale", scale);
}
