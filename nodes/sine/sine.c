#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_OSC_PRIV
#include "dsp/osc.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct sine_n {
    gf_cable *freq;
    gf_cable *amp;
    gf_cable *out;
    sk_osc osc;
    SKFLT tab[8192];
};

static void gen_sine(SKFLT *tab, int sz)
{
    SKFLT step;
    int i;

    step = 2 * M_PI / sz;

    for (i = 0; i < sz; i++) {
        tab[i] = sin(i * step);
    }
}

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct sine_n *sine;

    blksize = gf_node_blksize(node);

    sine = (struct sine_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, f, a;
        f = gf_cable_get(sine->freq, n);
        a = gf_cable_get(sine->amp, n);

        sk_osc_freq(&sine->osc, f);
        sk_osc_amp(&sine->osc, a);
        out = sk_osc_tick(&sine->osc);

        gf_cable_set(sine->out, n, out);
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

int sk_node_sine(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    sk_param amp;
    void *ud;
    struct sine_n *sine;
    int sr;

    rc = sk_param_get(core, &amp);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct sine_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    sine = (struct sine_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &sine->freq);
    gf_node_get_cable(node, 1, &sine->amp);
    gf_node_get_cable(node, 2, &sine->out);

    gf_node_set_data(node, sine);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sr = gf_patch_srate_get(patch);
    gen_sine(sine->tab, 8192);
    sk_osc_init(&sine->osc, sr, sine->tab, 8192, 0);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &amp, 1);
    sk_param_out(core, node, 2);
    return 0;
}
