#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#include "dsp/scale.h"

struct scale_n {
    gf_cable *in;
    gf_cable *min;
    gf_cable *max;
    gf_cable *out;
};

static void biscale_compute(gf_node *node)
{
    int blksize;
    int n;
    struct scale_n *scale;

    blksize = gf_node_blksize(node);

    scale = (struct scale_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, in, min, max;

        in = gf_cable_get(scale->in, n);
        min = gf_cable_get(scale->min, n);
        max = gf_cable_get(scale->max, n);
        out = sk_biscale(in, min, max);

        gf_cable_set(scale->out, n, out);
    }
}

static void scale_compute(gf_node *node)
{
    int blksize;
    int n;
    struct scale_n *scale;

    blksize = gf_node_blksize(node);

    scale = (struct scale_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, in, min, max;

        in = gf_cable_get(scale->in, n);
        min = gf_cable_get(scale->min, n);
        max = gf_cable_get(scale->max, n);
        out = sk_scale(in, min, max);

        gf_cable_set(scale->out, n, out);
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

static int node_scale(sk_core *core, gf_function compute)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param min;
    sk_param max;
    void *ud;
    struct scale_n *scale;

    rc = sk_param_get(core, &max);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &min);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct scale_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    scale = (struct scale_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &scale->in);
    gf_node_get_cable(node, 1, &scale->min);
    gf_node_get_cable(node, 2, &scale->max);
    gf_node_get_cable(node, 3, &scale->out);

    gf_node_set_data(node, scale);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &min, 1);
    sk_param_set(core, node, &max, 2);
    sk_param_out(core, node, 3);
    return 0;
}

int sk_node_biscale(sk_core *core)
{
    return node_scale(core, biscale_compute);
}

int sk_node_scale(sk_core *core)
{
    return node_scale(core, scale_compute);
}
