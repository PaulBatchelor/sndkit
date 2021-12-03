#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_sine(sk_core *core);

static lil_value_t sine(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "sine", argc, 2);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_sine(core);
    return NULL;
}

void sklil_load_sine(lil_t lil)
{
    lil_register(lil, "sine", sine);
}
