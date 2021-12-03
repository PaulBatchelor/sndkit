#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_rephasor(sk_core *core);
int sk_node_phsdiv(sk_core *core);
int sk_node_phsmul(sk_core *core);

static lil_value_t rephasor(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "rephasor", argc, 2);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_rephasor(core);
    return NULL;
}

static lil_value_t phsdiv(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "phsdiv", argc, 2);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_phsdiv(core);
    return NULL;
}

static lil_value_t phsmul(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "phsmul", argc, 2);

    sklil_param(core, argv[0]);
    sklil_param(core, argv[1]);

    sk_node_phsmul(core);
    return NULL;
}

void sklil_load_rephasor(lil_t lil)
{
    lil_register(lil, "rephasor", rephasor);
    lil_register(lil, "phsdiv", phsdiv);
    lil_register(lil, "phsmul", phsmul);
}
