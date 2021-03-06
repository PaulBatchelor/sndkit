#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#include "dsp/scale.h"

struct scale_n {
    pw_cable *in;
    pw_cable *min;
    pw_cable *max;
    pw_cable *out;
};

static void biscale_compute(pw_node *node)
{
    int blksize;
    int n;
    struct scale_n *scale;

    blksize = pw_node_blksize(node);

    scale = (struct scale_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, in, min, max;

        in = pw_cable_get(scale->in, n);
        min = pw_cable_get(scale->min, n);
        max = pw_cable_get(scale->max, n);
        out = sk_biscale(in, min, max);

        pw_cable_set(scale->out, n, out);
    }
}

static void scale_compute(pw_node *node)
{
    int blksize;
    int n;
    struct scale_n *scale;

    blksize = pw_node_blksize(node);

    scale = (struct scale_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, in, min, max;

        in = pw_cable_get(scale->in, n);
        min = pw_cable_get(scale->min, n);
        max = pw_cable_get(scale->max, n);
        out = sk_scale(in, min, max);

        pw_cable_set(scale->out, n, out);
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

static int node_scale(sk_core *core, pw_function compute)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct scale_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    scale = (struct scale_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &scale->in);
    pw_node_get_cable(node, 1, &scale->min);
    pw_node_get_cable(node, 2, &scale->max);
    pw_node_get_cable(node, 3, &scale->out);

    pw_node_set_data(node, scale);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

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
