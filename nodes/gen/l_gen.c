#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

int sk_node_gensine(sk_core *core);
int sk_node_gensaw(sk_core *core);
int sk_tab_sinesum(sk_core *core, const char *argstr);
int sk_tab_vals(sk_core *core, const char *argstr);

static lil_value_t gensine(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "gensine", argc, 1);

    rc = sk_node_gensine(core);

    SKLIL_ERROR_CHECK(lil, rc, "gensine didn't work out.");

    return NULL;
}

static lil_value_t gensaw(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "gensaw", argc, 1);

    rc = sk_node_gensaw(core);

    SKLIL_ERROR_CHECK(lil, rc, "gensaw didn't work out.");

    return NULL;
}

static lil_value_t gensinesum(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    const char *str;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "gensinesum", argc, 2);

    str = lil_to_string(argv[1]);

    rc = sk_tab_sinesum(core, str);

    SKLIL_ERROR_CHECK(lil, rc, "gensinesum didn't work out.");

    return NULL;
}

static lil_value_t genvals(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    const char *str;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "genvals", argc, 2);

    str = lil_to_string(argv[1]);

    rc = sk_tab_vals(core, str);

    SKLIL_ERROR_CHECK(lil, rc, "genvals didn't work out.");

    return NULL;
}

void sklil_load_gen(lil_t lil)
{
    lil_register(lil, "gensine", gensine);
    lil_register(lil, "gensaw", gensaw);
    lil_register(lil, "gensinesum", gensinesum);
    lil_register(lil, "genvals", genvals);
}
