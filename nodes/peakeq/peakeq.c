#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_PEAKEQ_PRIV
#include "dsp/peakeq.h"

struct peakeq_n {
    gf_cable *in;
    gf_cable *freq;
    gf_cable *bw;
    gf_cable *gain;
    gf_cable *out;
    sk_peakeq peakeq;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct peakeq_n *peakeq;

    blksize = gf_node_blksize(node);

    peakeq = (struct peakeq_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, bw, gain, out;
        in = gf_cable_get(peakeq->in, n);

        freq = gf_cable_get(peakeq->freq, n);
        sk_peakeq_freq(&peakeq->peakeq, freq);

        bw = gf_cable_get(peakeq->bw, n);
        sk_peakeq_bandwidth(&peakeq->peakeq, bw);

        gain = gf_cable_get(peakeq->gain, n);
        sk_peakeq_gain(&peakeq->peakeq, gain);

        out = sk_peakeq_tick(&peakeq->peakeq, in);
        gf_cable_set(peakeq->out, n, out);
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

int sk_node_peakeq(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, freq, bw, gain;
    void *ud;
    struct peakeq_n *peakeq;
    int sr;

    rc = sk_param_get(core, &gain);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &bw);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct peakeq_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    peakeq = (struct peakeq_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_peakeq_init(&peakeq->peakeq, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &peakeq->in);
    gf_node_get_cable(node, 1, &peakeq->freq);
    gf_node_get_cable(node, 2, &peakeq->bw);
    gf_node_get_cable(node, 3, &peakeq->gain);
    gf_node_get_cable(node, 4, &peakeq->out);

    gf_node_set_data(node, peakeq);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &bw, 2);
    sk_param_set(core, node, &gain, 3);
    sk_param_out(core, node, 4);
    return 0;
}
