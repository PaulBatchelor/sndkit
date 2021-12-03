#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_CLKPHS_PRIV
#include "dsp/clkphs.h"

struct clkphs_n {
    gf_cable *in;
    gf_cable *out;
    sk_clkphs clkphs;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct clkphs_n *clkphs;

    blksize = gf_node_blksize(node);

    clkphs = (struct clkphs_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, out;

        in = gf_cable_get(clkphs->in, n);

        out = sk_clkphs_tick(&clkphs->clkphs, in);

        gf_cable_set(clkphs->out, n, out);
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

int gf_node_clkphs(gf_node *node)
{
    int rc;
    gf_patch *patch;
    void *ud;
    struct clkphs_n *clkphs;

    rc = gf_node_get_patch(node, &patch);

    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct clkphs_n), &ud);
    if (rc != GF_OK) return rc;

    clkphs = (struct clkphs_n *)ud;

    sk_clkphs_init(&clkphs->clkphs);

    rc = gf_node_cables_alloc(node, 2);
    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &clkphs->in);
    gf_node_get_cable(node, 1, &clkphs->out);

    gf_node_set_data(node, clkphs);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_clkphs(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_clkphs(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);

    return 0;
}
