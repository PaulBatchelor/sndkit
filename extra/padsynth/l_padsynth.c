#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_padsynth(sk_core *core);

static lil_value_t padsynth(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "padsynth", argc, 4);


    sklil_param(core, argv[2]);
    sklil_param(core, argv[3]);

    rc = sk_padsynth(core);
    SKLIL_ERROR_CHECK(lil, rc, "padsynth didn't work out.");

    return NULL;
}

void sklil_load_padsynth(lil_t lil)
{
    lil_register(lil, "padsynth", padsynth);
}
