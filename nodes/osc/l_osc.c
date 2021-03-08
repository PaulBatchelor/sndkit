#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"
#include "osc.h"

static lil_value_t osc(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "osc", argc, 4);

    /* skip param 0 */
    sklil_param(core, argv[1]);
    sklil_param(core, argv[2]);
    sklil_param(core, argv[3]);

    rc = sk_node_osc(core);

    if (rc) {
        /* TODO: error handling */
    }
    return NULL;
}

void sklil_load_osc(lil_t lil)
{
    lil_register(lil, "osc", osc);
}
