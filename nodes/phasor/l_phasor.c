#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_phasor(sk_core *core, SKFLT iphs);
int sk_node_tphasor(sk_core *core, SKFLT iphs);

static lil_value_t phasor(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "phasor", argc, 2);

    sklil_param(core, argv[0]);

    sk_node_phasor(core, lil_to_double(argv[1]));
    return NULL;
}

static lil_value_t tphasor(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tphasor", argc, 3);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_tphasor(core, lil_to_double(argv[2]));
    return NULL;
}

void sklil_load_phasor(lil_t lil)
{
    lil_register(lil, "phasor", phasor);
    lil_register(lil, "tphasor", tphasor);
}
