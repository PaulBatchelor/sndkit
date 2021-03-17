#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_EXPMAP_PRIV
#include "dsp/expmap.h"

struct expmap_n {
    pw_cable *in;
    pw_cable *slope;
    pw_cable *out;
    sk_expmap expmap;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct expmap_n *expmap;

    blksize = pw_node_blksize(node);

    expmap = (struct expmap_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, out, slope;
        in = pw_cable_get(expmap->in, n);
        slope = pw_cable_get(expmap->slope, n);
        sk_expmap_slope(&expmap->expmap, slope);
        out = sk_expmap_tick(&expmap->expmap, in);
        pw_cable_set(expmap->out, n, out);
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

int sk_node_expmap(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, slope;
    void *ud;
    struct expmap_n *expmap;

    rc = sk_param_get(core, &slope);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct expmap_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    expmap = (struct expmap_n *)ud;

    sk_expmap_init(&expmap->expmap);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &expmap->in);
    pw_node_get_cable(node, 1, &expmap->slope);
    pw_node_get_cable(node, 2, &expmap->out);

    pw_node_set_data(node, expmap);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &slope, 1);
    sk_param_out(core, node, 2);
    return 0;
}
