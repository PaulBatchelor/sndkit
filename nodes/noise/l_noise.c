#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_noise(sk_core *core);

static lil_value_t noise(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    core = lil_get_data(lil);

    rc = sk_node_noise(core);
    SKLIL_ERROR_CHECK(lil, rc, "noise didn't work out.");
    return NULL;
}

void sklil_load_noise(lil_t lil)
{
    lil_register(lil, "noise", noise);
}
