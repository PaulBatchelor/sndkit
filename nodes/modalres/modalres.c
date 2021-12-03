#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_MODALRES_PRIV
#include "dsp/modalres.h"

struct modalres_n {
    gf_cable *in;
    gf_cable *freq;
    gf_cable *q;
    gf_cable *out;
    sk_modalres modalres;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct modalres_n *modalres;

    blksize = gf_node_blksize(node);

    modalres = (struct modalres_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, q, out;
        in = gf_cable_get(modalres->in, n);

        freq = gf_cable_get(modalres->freq, n);
        sk_modalres_freq(&modalres->modalres, freq);
        q = gf_cable_get(modalres->q, n);
        sk_modalres_q(&modalres->modalres, q);

        out = sk_modalres_tick(&modalres->modalres, in);
        gf_cable_set(modalres->out, n, out);
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

int sk_node_modalres(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, freq, q;
    void *ud;
    struct modalres_n *modalres;
    int sr;

    rc = sk_param_get(core, &q);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct modalres_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    modalres = (struct modalres_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_modalres_init(&modalres->modalres, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &modalres->in);
    gf_node_get_cable(node, 1, &modalres->freq);
    gf_node_get_cable(node, 2, &modalres->q);
    gf_node_get_cable(node, 3, &modalres->out);

    gf_node_set_data(node, modalres);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &q, 2);
    sk_param_out(core, node, 3);
    return 0;
}
