#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "phasor.h"

static lil_value_t phasor(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "phasor", argc, 2);

    sklil_param(core, argv[0]);

    sk_node_phasor(core, lil_to_double(argv[1]));
    return NULL;
}

void sklil_load_phasor(lil_t lil)
{
    lil_register(lil, "phasor", phasor);
}
