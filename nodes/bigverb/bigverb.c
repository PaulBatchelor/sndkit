#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_BIGVERB_PRIV
#include "dsp/bigverb.h"

struct bigverb_n {
    gf_cable *in[2];
    gf_cable *size;
    gf_cable *cutoff;
    gf_cable *out[2];
    sk_bigverb *bigverb;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct bigverb_n *bigverb;

    blksize = gf_node_blksize(node);

    bigverb = (struct bigverb_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in[2], size, cutoff, out[2];
        in[0] = gf_cable_get(bigverb->in[0], n);
        in[1] = gf_cable_get(bigverb->in[1], n);
        size = gf_cable_get(bigverb->size, n);
        cutoff = gf_cable_get(bigverb->cutoff, n);

        sk_bigverb_size(bigverb->bigverb, size);
        sk_bigverb_cutoff(bigverb->bigverb, cutoff);
        sk_bigverb_tick(bigverb->bigverb,
                        in[0], in[1],
                        &out[0], &out[1]);

        gf_cable_set(bigverb->out[0], n, out[0]);
        gf_cable_set(bigverb->out[1], n, out[1]);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct bigverb_n *bv;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    bv = ud;
    sk_bigverb_del(bv->bigverb);
    gf_memory_free(patch, &ud);
}

int sk_node_bigverb(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in[2];
    sk_param size;
    sk_param cutoff;
    void *ud;
    struct bigverb_n *bigverb;
    int sr;

    rc = sk_param_get(core, &cutoff);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &size);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in[1]);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in[0]);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct bigverb_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    bigverb = (struct bigverb_n *)ud;

    sr = gf_patch_srate_get(patch);
    bigverb->bigverb = sk_bigverb_new(sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 6);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);
    gf_node_set_block(node, 5);

    gf_node_get_cable(node, 0, &bigverb->in[0]);
    gf_node_get_cable(node, 1, &bigverb->in[1]);
    gf_node_get_cable(node, 2, &bigverb->size);
    gf_node_get_cable(node, 3, &bigverb->cutoff);
    gf_node_get_cable(node, 4, &bigverb->out[0]);
    gf_node_get_cable(node, 5, &bigverb->out[1]);

    gf_node_set_data(node, bigverb);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in[0], 0);
    sk_param_set(core, node, &in[1], 1);
    sk_param_set(core, node, &size, 2);
    sk_param_set(core, node, &cutoff, 3);
    sk_param_out(core, node, 4);
    sk_param_out(core, node, 5);
    return 0;
}
