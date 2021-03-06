#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_OSC_PRIV
#include "sine.h"
#include "dsp/osc.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct sine_n {
    pw_cable *freq;
    pw_cable *amp;
    pw_cable *out;
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

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct sine_n *sine;

    blksize = pw_node_blksize(node);

    sine = (struct sine_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, f, a;
        f = pw_cable_get(sine->freq, n);
        a = pw_cable_get(sine->amp, n);

        sk_osc_freq(&sine->osc, f);
        sk_osc_amp(&sine->osc, a);
        out = sk_osc_tick(&sine->osc);

        pw_cable_set(sine->out, n, out);
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

int sk_node_sine(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct sine_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    sine = (struct sine_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &sine->freq);
    pw_node_get_cable(node, 1, &sine->amp);
    pw_node_get_cable(node, 2, &sine->out);

    pw_node_set_data(node, sine);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sr = pw_patch_srate_get(patch);
    gen_sine(sine->tab, 8192);
    sk_osc_init(&sine->osc, sr, sine->tab, 8192, 0);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &amp, 1);
    sk_param_out(core, node, 2);
    return 0;
}
