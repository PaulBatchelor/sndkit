#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sklil_main(int argc, char *argv[])
{
    return lil_main(argc, argv, sklil_loader, sklil_clean);
}

int sklil_param(sk_core *core, lil_value_t arg)
{
    const char *sarg;
    sarg = lil_to_string(arg);

    if (!strcmp(sarg, "zz") || strlen(sarg) == 0) {
        return 0;
    }

    return sk_core_constant(core, lil_to_double(arg));
}
