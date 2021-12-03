#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_BEZIER_PRIV
#include "dsp/bezier.h"

struct bezier_n {
    gf_cable *in;
    gf_cable *cx;
    gf_cable *cy;
    gf_cable *out;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct bezier_n *bezier;

    blksize = gf_node_blksize(node);

    bezier = (struct bezier_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, cx, cy, out;
        in = gf_cable_get(bezier->in, n);
        cx = gf_cable_get(bezier->cx, n);
        cy = gf_cable_get(bezier->cy, n);
        out = sk_bezier_tick(in, cx, cy);
        gf_cable_set(bezier->out, n, out);
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

int sk_node_bezier(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, cx, cy;
    void *ud;
    struct bezier_n *bezier;

    rc = sk_param_get(core, &cy);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &cx);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct bezier_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    bezier = (struct bezier_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &bezier->in);
    gf_node_get_cable(node, 1, &bezier->cx);
    gf_node_get_cable(node, 2, &bezier->cy);
    gf_node_get_cable(node, 3, &bezier->out);

    gf_node_set_data(node, bezier);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &cx, 1);
    sk_param_set(core, node, &cy, 2);
    sk_param_out(core, node, 3);
    return 0;
}
