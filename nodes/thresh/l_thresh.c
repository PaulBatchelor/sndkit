#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_thresh(sk_core *core);

static lil_value_t thresh(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    int n;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "thresh", argc, 3);

    for (n = 0; n < 3; n++) {
        rc = sklil_param(core, argv[n]);
        SKLIL_PARAM_CHECK(lil, rc, "thresh");
    }

    rc = sk_node_thresh(core);
    SKLIL_ERROR_CHECK(lil, rc, "thresh didn't work out.");
    return NULL;
}

void sklil_load_thresh(lil_t lil)
{
    lil_register(lil, "thresh", thresh);
}
