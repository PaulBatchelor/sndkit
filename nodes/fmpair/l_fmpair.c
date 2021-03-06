#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "fmpair.h"

static lil_value_t fmpair(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "fmpair", argc, 6);

    /* skip param 0 */
    rc = sklil_param(core, argv[1]);
    SKLIL_PARAM_CHECK(lil, rc, "fmpair");
    rc = sklil_param(core, argv[2]);
    SKLIL_PARAM_CHECK(lil, rc, "fmpair");
    rc = sklil_param(core, argv[3]);
    SKLIL_PARAM_CHECK(lil, rc, "fmpair");
    rc = sklil_param(core, argv[4]);
    SKLIL_PARAM_CHECK(lil, rc, "fmpair");
    rc = sklil_param(core, argv[5]);
    SKLIL_PARAM_CHECK(lil, rc, "fmpair");

    rc = sk_node_fmpair(core);

    SKLIL_ERROR_CHECK(lil, rc, "fmpair did not work out.");

    return NULL;
}

void sklil_load_fmpair(lil_t lil)
{
    lil_register(lil, "fmpair", fmpair);
}
