#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_loadwav(sk_core *core, const char *filename);

static lil_value_t loadwav(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "loadwav", argc, 1);

    sk_loadwav(core, lil_to_string(argv[0]));
    return NULL;
}

void sklil_load_loadwav(lil_t lil)
{
    lil_register(lil, "loadwav", loadwav);
}
