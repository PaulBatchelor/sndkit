#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_THRESH_PRIV
#include "dsp/thresh.h"

struct thresh_n {
    gf_cable *in;
    gf_cable *value;
    gf_cable *mode;
    gf_cable *out;
    sk_thresh thresh;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct thresh_n *thresh;

    blksize = gf_node_blksize(node);

    thresh = (struct thresh_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, value, mode, out;
        in = gf_cable_get(thresh->in, n);
        value = gf_cable_get(thresh->value, n);
        mode = gf_cable_get(thresh->mode, n);

        sk_thresh_value(&thresh->thresh, value);
        sk_thresh_mode(&thresh->thresh, (int)mode);
        out = sk_thresh_tick(&thresh->thresh, in);
        gf_cable_set(thresh->out, n, out);
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

int sk_node_thresh(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param val;
    sk_param mode;
    void *ud;
    struct thresh_n *thresh;

    rc = sk_param_get(core, &mode);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &val);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct thresh_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    thresh = (struct thresh_n *)ud;

    sk_thresh_init(&thresh->thresh);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &thresh->in);
    gf_node_get_cable(node, 1, &thresh->value);
    gf_node_get_cable(node, 2, &thresh->mode);
    gf_node_get_cable(node, 3, &thresh->out);

    gf_node_set_data(node, thresh);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &val, 1);
    sk_param_set(core, node, &mode, 2);
    sk_param_out(core, node, 3);
    return 0;
}
