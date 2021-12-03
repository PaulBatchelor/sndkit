#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_SMOOTHER_PRIV
#include "dsp/smoother.h"

struct smoother_n {
    gf_cable *in;
    gf_cable *trig;
    gf_cable *smooth;
    gf_cable *out;
    sk_smoother smoother;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct smoother_n *smoother;

    blksize = gf_node_blksize(node);

    smoother = (struct smoother_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, smooth, in;
        in = gf_cable_get(smoother->in, n);
        smooth = gf_cable_get(smoother->smooth, n);

        sk_smoother_time(&smoother->smoother, smooth);
        out = sk_smoother_tick(&smoother->smoother, in);
        gf_cable_set(smoother->out, n, out);
    }
}

static void compute_withtrig(gf_node *node)
{
    int blksize;
    int n;
    struct smoother_n *smoother;

    blksize = gf_node_blksize(node);

    smoother = (struct smoother_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, smooth, in, trig;
        in = gf_cable_get(smoother->in, n);
        trig = gf_cable_get(smoother->trig, n);
        smooth = gf_cable_get(smoother->smooth, n);

        sk_smoother_time(&smoother->smoother, smooth);

        if (trig != 0) {
            sk_smoother_reset(&smoother->smoother, in);
        }

        out = sk_smoother_tick(&smoother->smoother, in);

        gf_cable_set(smoother->out, n, out);
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

int sk_node_smoother(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param smooth;
    void *ud;
    struct smoother_n *smoother;
    int sr;

    rc = sk_param_get(core, &smooth);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct smoother_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    smoother = (struct smoother_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_smoother_init(&smoother->smoother, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &smoother->in);
    gf_node_get_cable(node, 1, &smoother->smooth);
    gf_node_get_cable(node, 2, &smoother->out);

    gf_node_set_data(node, smoother);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &smooth, 1);
    sk_param_out(core, node, 2);
    return 0;
}

int sk_node_tsmoother(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param trig;
    sk_param smooth;
    void *ud;
    struct smoother_n *smoother;
    int sr;

    rc = sk_param_get(core, &smooth);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct smoother_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    smoother = (struct smoother_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_smoother_init(&smoother->smoother, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &smoother->in);
    gf_node_get_cable(node, 1, &smoother->trig);
    gf_node_get_cable(node, 2, &smoother->smooth);
    gf_node_get_cable(node, 3, &smoother->out);

    gf_node_set_data(node, smoother);
    gf_node_set_compute(node, compute_withtrig);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &trig, 1);
    sk_param_set(core, node, &smooth, 2);
    sk_param_out(core, node, 3);
    return 0;
}
