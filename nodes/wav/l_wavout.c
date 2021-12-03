#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_wavout(sk_core *core, const char *filename);
int sk_node_wavouts(sk_core *core, const char *filename);

static lil_value_t wavout(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "wavout", argc, 2);

    sklil_param(core, argv[0]);

    sk_node_wavout(core, lil_to_string(argv[1]));
    return NULL;
}

static lil_value_t wavouts(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "wavouts", argc, 3);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_wavouts(core, lil_to_string(argv[2]));
    return NULL;
}

void sklil_load_wavout(lil_t lil)
{
    lil_register(lil, "wavout", wavout);
    lil_register(lil, "wavouts", wavouts);
}
