#include "graforge.h"
#include "core.h"
#include "dsp/chorus.h"

struct chorus_n {
    gf_cable *in;
    gf_cable *rate;
    gf_cable *depth;
    gf_cable *mix;
    gf_cable *out;
    sk_chorus *chorus;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct chorus_n *chorus;

    blksize = gf_node_blksize(node);

    chorus = (struct chorus_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, rate, depth, mix, out;

        in = gf_cable_get(chorus->in, n);
        rate = gf_cable_get(chorus->rate, n);
        sk_chorus_rate(chorus->chorus, rate);
        depth = gf_cable_get(chorus->depth, n);
        sk_chorus_depth(chorus->chorus, depth);
        mix = gf_cable_get(chorus->mix, n);
        sk_chorus_mix(chorus->chorus, mix);

        out = sk_chorus_tick(chorus->chorus, in);

        gf_cable_set(chorus->out, n, out);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct chorus_n *c;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    c = ud;
    sk_chorus_del(c->chorus);
    gf_memory_free(patch, &ud);
}

int sk_node_chorus(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, rate, depth, mix;
    SKFLT delay;
    struct chorus_n *chorus;
    int sr;
    void *ud;

    rc = sk_param_get_constant(core, &delay);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &mix);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &depth);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct chorus_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    chorus = (struct chorus_n *)ud;

    sr = gf_patch_srate_get(patch);
    chorus->chorus = sk_chorus_new(sr, delay);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &chorus->in);
    gf_node_get_cable(node, 1, &chorus->rate);
    gf_node_get_cable(node, 2, &chorus->depth);
    gf_node_get_cable(node, 3, &chorus->mix);
    gf_node_get_cable(node, 4, &chorus->out);

    gf_node_set_data(node, chorus);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &rate, 1);
    sk_param_set(core, node, &depth, 2);
    sk_param_set(core, node, &mix, 3);
    sk_param_out(core, node, 4);

    return 0;
}
