#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_BEZIER_PRIV
#include "dsp/bezier.h"

struct bezier_n {
    pw_cable *in;
    pw_cable *cx;
    pw_cable *cy;
    pw_cable *out;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct bezier_n *bezier;

    blksize = pw_node_blksize(node);

    bezier = (struct bezier_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, cx, cy, out;
        in = pw_cable_get(bezier->in, n);
        cx = pw_cable_get(bezier->cx, n);
        cy = pw_cable_get(bezier->cy, n);
        out = sk_bezier_tick(in, cx, cy);
        pw_cable_set(bezier->out, n, out);
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

int sk_node_bezier(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct bezier_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    bezier = (struct bezier_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &bezier->in);
    pw_node_get_cable(node, 1, &bezier->cx);
    pw_node_get_cable(node, 2, &bezier->cy);
    pw_node_get_cable(node, 3, &bezier->out);

    pw_node_set_data(node, bezier);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &cx, 1);
    sk_param_set(core, node, &cy, 2);
    sk_param_out(core, node, 3);
    return 0;
}
