#include "graforge.h"
#include "core.h"
#include "dsp/gen.h"

int sk_node_gensine(sk_core *core)
{
    sk_table *tab;
    int rc;

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);
    sk_gen_sine(sk_table_data(tab), sk_table_size(tab));
    rc = sk_core_table_push(core, tab);
    SK_ERROR_CHECK(rc);
    return 0;
}

int sk_node_gensaw(sk_core *core)
{
    sk_table *tab;
    int rc;

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);
    sk_gen_saw(sk_table_data(tab), sk_table_size(tab));
    rc = sk_core_table_push(core, tab);
    SK_ERROR_CHECK(rc);
    return 0;
}

int sk_tab_sinesum(sk_core *core, const char *argstr)
{
    sk_table *tab;
    int rc;

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);


    sk_gen_sinesum(sk_table_data(tab),
                   sk_table_size(tab),
                   argstr);
    rc = sk_core_table_push(core, tab);
    SK_ERROR_CHECK(rc);
    return 0;
}

int sk_tab_vals(sk_core *core, const char *argstr)
{
    sk_table *tab;
    int rc;
    SKFLT *data;
    int sz;

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    data = sk_table_data(tab);
    sz = sk_table_size(tab);
    sk_gen_vals(&data, &sz, argstr);

    sk_table_init(tab, data, sz);

    rc = sk_core_table_push(core, tab);
    SK_ERROR_CHECK(rc);
    return 0;
}
