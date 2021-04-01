#include <sndkit/patchwerk.h>
#include <sndkit/core.h>

#include "dsp/chorus.h"

struct chorus_n {
    pw_cable *in;
    pw_cable *rate;
    pw_cable *depth;
    pw_cable *mix;
    pw_cable *out;
    sk_chorus *chorus;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct chorus_n *chorus;

    blksize = pw_node_blksize(node);

    chorus = (struct chorus_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, rate, depth, mix, out;

        in = pw_cable_get(chorus->in, n);
        rate = pw_cable_get(chorus->rate, n);
        sk_chorus_rate(chorus->chorus, rate);
        depth = pw_cable_get(chorus->depth, n);
        sk_chorus_depth(chorus->chorus, depth);
        mix = pw_cable_get(chorus->mix, n);
        sk_chorus_mix(chorus->chorus, mix);

        out = sk_chorus_tick(chorus->chorus, in);

        pw_cable_set(chorus->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    struct chorus_n *c;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    c = ud;
    sk_chorus_del(c->chorus);
    pw_memory_free(patch, &ud);
}

int sk_node_chorus(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct chorus_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    chorus = (struct chorus_n *)ud;

    sr = pw_patch_srate_get(patch);
    chorus->chorus = sk_chorus_new(sr, delay);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 5);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 4);

    pw_node_get_cable(node, 0, &chorus->in);
    pw_node_get_cable(node, 1, &chorus->rate);
    pw_node_get_cable(node, 2, &chorus->depth);
    pw_node_get_cable(node, 3, &chorus->mix);
    pw_node_get_cable(node, 4, &chorus->out);

    pw_node_set_data(node, chorus);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &rate, 1);
    sk_param_set(core, node, &depth, 2);
    sk_param_set(core, node, &mix, 3);
    sk_param_out(core, node, 4);

    return 0;
}
