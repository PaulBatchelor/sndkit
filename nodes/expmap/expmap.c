#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_EXPMAP_PRIV
#include "dsp/expmap.h"

struct expmap_n {
    gf_cable *in;
    gf_cable *slope;
    gf_cable *out;
    sk_expmap expmap;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct expmap_n *expmap;

    blksize = gf_node_blksize(node);

    expmap = (struct expmap_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, out, slope;
        in = gf_cable_get(expmap->in, n);
        slope = gf_cable_get(expmap->slope, n);
        sk_expmap_slope(&expmap->expmap, slope);
        out = sk_expmap_tick(&expmap->expmap, in);
        gf_cable_set(expmap->out, n, out);
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

int sk_node_expmap(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, slope;
    void *ud;
    struct expmap_n *expmap;

    rc = sk_param_get(core, &slope);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct expmap_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    expmap = (struct expmap_n *)ud;

    sk_expmap_init(&expmap->expmap);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &expmap->in);
    gf_node_get_cable(node, 1, &expmap->slope);
    gf_node_get_cable(node, 2, &expmap->out);

    gf_node_set_data(node, expmap);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &slope, 1);
    sk_param_out(core, node, 2);
    return 0;
}
