#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_wavin(sk_core *core, const char *filename);

static lil_value_t wavin(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "wavin", argc, 1);

    sk_node_wavin(core, lil_to_string(argv[0]));
    return NULL;
}

void sklil_load_wavin(lil_t lil)
{
    lil_register(lil, "wavin", wavin);
}
