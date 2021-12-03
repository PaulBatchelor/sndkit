#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_BUTTERWORTH_PRIV
#include "dsp/butterworth.h"

struct butterworth_n {
    gf_cable *in;
    gf_cable *freq;
    gf_cable *out;
    sk_butterworth butterworth;
};

static void compute(gf_node *node,
                    SKFLT (tick)(sk_butterworth*,SKFLT))
{
    int blksize;
    int n;
    struct butterworth_n *butterworth;

    blksize = gf_node_blksize(node);

    butterworth = (struct butterworth_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, out;
        in = gf_cable_get(butterworth->in, n);
        freq = gf_cable_get(butterworth->freq, n);
        sk_butterworth_freq(&butterworth->butterworth, freq);

        out = tick(&butterworth->butterworth, in);
        gf_cable_set(butterworth->out, n, out);
    }
}

static void butlp(gf_node *node)
{
    compute(node, sk_butlp_tick);
}

static void buthp(gf_node *node)
{
    compute(node, sk_buthp_tick);
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

static int butterworth_node(sk_core *core, gf_function compute)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, freq;
    void *ud;
    struct butterworth_n *butterworth;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct butterworth_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    butterworth = (struct butterworth_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_butterworth_init(&butterworth->butterworth, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &butterworth->in);
    gf_node_get_cable(node, 1, &butterworth->freq);
    gf_node_get_cable(node, 2, &butterworth->out);

    gf_node_set_data(node, butterworth);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_out(core, node, 2);
    return 0;
}

int sk_node_butlp(sk_core *core)
{
    return butterworth_node(core, butlp);
}

int sk_node_buthp(sk_core *core)
{
    return butterworth_node(core, buthp);
}
