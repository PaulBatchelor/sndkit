#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"

static lil_value_t tabnew(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    unsigned long sz;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tabnew", argc, 1);

    sz = lil_to_integer(argv[0]);

    rc = sk_core_table_new(core, sz);

    if (rc) {
        /* TODO: error handling */
    }
    return NULL;
}

void sklil_load_tabnew(lil_t lil)
{
    lil_register(lil, "tabnew", tabnew);
}
