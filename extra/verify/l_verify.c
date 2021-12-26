#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_verify(sk_core *core, char *out);
void lil_set_errcode(int err);

static lil_value_t l_verify(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    char md5[33];
    const char *cmp;
    int rc;

    core = lil_get_data(lil);

    rc = sk_verify(core, md5);

    SKLIL_ERROR_CHECK(lil, rc, "verify: empty stack");

    if (argc == 0) {
        printf("%s\n", md5);
        return NULL;
    }

    cmp = lil_to_string(argv[0]);

    if (strcmp(cmp, md5)) {
        /* technically not secure. try not to stack smash this. */
        char str[128];
        sprintf(str, "expected %s, got %s", cmp, md5);
        lil_set_error(lil, str);
        lil_set_errcode(1);
        return NULL;
    }


    return NULL;
}

void sklil_load_verify(lil_t lil)
{
    lil_register(lil, "verify", l_verify);
}
