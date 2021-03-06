#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "arith.h"

static lil_value_t add(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "add", argc, 2);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_add(core);
    return NULL;
}

void sklil_load_arith(lil_t lil)
{
    lil_register(lil, "add", add);
}
