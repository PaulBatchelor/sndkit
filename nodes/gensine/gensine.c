#include "patchwerk/patchwerk.h"
#include "core.h"
#include "gensine.h"
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
