#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_plotter(sk_core *core, const char *filename);

static lil_value_t plotter(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "plotter", argc, 3);

    rc = sklil_param(core, argv[0]);
    SKLIL_PARAM_CHECK(lil, rc, "plotter");
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "plotter");

    rc = sk_node_plotter(core, lil_to_string(argv[2]));
    SKLIL_ERROR_CHECK(lil, rc, "plotter didn't work out.");
    return NULL;
}

void sklil_load_plotter(lil_t lil)
{
    lil_register(lil, "plotter", plotter);
}
