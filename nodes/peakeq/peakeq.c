#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_PEAKEQ_PRIV
#include "dsp/peakeq.h"

struct peakeq_n {
    pw_cable *in;
    pw_cable *freq;
    pw_cable *bw;
    pw_cable *gain;
    pw_cable *out;
    sk_peakeq peakeq;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct peakeq_n *peakeq;

    blksize = pw_node_blksize(node);

    peakeq = (struct peakeq_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, freq, bw, gain, out;
        in = pw_cable_get(peakeq->in, n);

        freq = pw_cable_get(peakeq->freq, n);
        sk_peakeq_freq(&peakeq->peakeq, freq);

        bw = pw_cable_get(peakeq->bw, n);
        sk_peakeq_bandwidth(&peakeq->peakeq, bw);

        gain = pw_cable_get(peakeq->gain, n);
        sk_peakeq_gain(&peakeq->peakeq, gain);

        out = sk_peakeq_tick(&peakeq->peakeq, in);
        pw_cable_set(peakeq->out, n, out);
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

int sk_node_peakeq(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct peakeq_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    peakeq = (struct peakeq_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_peakeq_init(&peakeq->peakeq, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 5);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 4);

    pw_node_get_cable(node, 0, &peakeq->in);
    pw_node_get_cable(node, 1, &peakeq->freq);
    pw_node_get_cable(node, 2, &peakeq->bw);
    pw_node_get_cable(node, 3, &peakeq->gain);
    pw_node_get_cable(node, 4, &peakeq->out);

    pw_node_set_data(node, peakeq);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &bw, 2);
    sk_param_set(core, node, &gain, 3);
    sk_param_out(core, node, 4);
    return 0;
}
