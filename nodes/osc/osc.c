#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_OSC_PRIV
#include "dsp/osc.h"

struct osc_n {
    pw_cable *freq;
    pw_cable *amp;
    pw_cable *out;
    sk_osc osc;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct osc_n *osc;

    blksize = pw_node_blksize(node);

    osc = (struct osc_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, f, a;
        f = pw_cable_get(osc->freq, n);
        a = pw_cable_get(osc->amp, n);

        sk_osc_freq(&osc->osc, f);
        sk_osc_amp(&osc->osc, a);
        out = sk_osc_tick(&osc->osc);

        pw_cable_set(osc->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    pw_memory_free(patch, &ud);
}

int sk_node_osc(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq;
    sk_param amp;
    void *ud;
    struct osc_n *osc;
    int sr;
    sk_table *tab;
    SKFLT iphs;

    rc = sk_param_get_constant(core, &iphs);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &amp);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct osc_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    osc = (struct osc_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_osc_init(&osc->osc, sr,
                sk_table_data(tab),
                sk_table_size(tab),
                iphs);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &osc->freq);
    pw_node_get_cable(node, 1, &osc->amp);
    pw_node_get_cable(node, 2, &osc->out);

    pw_node_set_data(node, osc);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &amp, 1);
    sk_param_out(core, node, 2);
    return 0;
}
