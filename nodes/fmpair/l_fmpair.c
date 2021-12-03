#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_fmpair(sk_core *core);
int sk_node_fmpair2(sk_core *core);

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

static lil_value_t fmpair2(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int i;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "fmpair2", argc, 6);

    /* skip param 0 + 1 */

    for (i = 2; i < 7; i++) {
        rc = sklil_param(core, argv[i]);
        SKLIL_PARAM_CHECK(lil, rc, "fmpair2");
    }

    rc = sk_node_fmpair2(core);

    SKLIL_ERROR_CHECK(lil, rc, "fmpair2 did not work out.");

    return NULL;
}

void sklil_load_fmpair(lil_t lil)
{
    lil_register(lil, "fmpair", fmpair);
    lil_register(lil, "fmpair2", fmpair2);
}
