#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_OSC_PRIV
#include "dsp/osc.h"

struct osc_n {
    gf_cable *freq;
    gf_cable *amp;
    gf_cable *out;
    sk_osc osc;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct osc_n *osc;

    blksize = gf_node_blksize(node);

    osc = (struct osc_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, f, a;
        f = gf_cable_get(osc->freq, n);
        a = gf_cable_get(osc->amp, n);

        sk_osc_freq(&osc->osc, f);
        sk_osc_amp(&osc->osc, a);
        out = sk_osc_tick(&osc->osc);

        gf_cable_set(osc->out, n, out);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    gf_memory_free(patch, &ud);
}

int sk_node_osc(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
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

    rc = gf_memory_alloc(patch, sizeof(struct osc_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    osc = (struct osc_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_osc_init(&osc->osc, sr,
                sk_table_data(tab),
                sk_table_size(tab),
                iphs);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &osc->freq);
    gf_node_get_cable(node, 1, &osc->amp);
    gf_node_get_cable(node, 2, &osc->out);

    gf_node_set_data(node, osc);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &amp, 1);
    sk_param_out(core, node, 2);
    return 0;
}
