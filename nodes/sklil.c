#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"

void sklil_param(sk_core *core, lil_value_t arg)
{
    const char *sarg;
    sarg = lil_to_string(arg);

    if (!strcmp(sarg, "zz") || strlen(sarg) == 0) {
        return;
    }

    sk_core_constant(core, lil_to_double(arg));
}
