/*
 * Prev
 *
 * Simply allocates a cable and does nothing to it.
 * This allows for signals to read the previous block
 * contents.
 *
 */

#include "graforge.h"
#include "core.h"

static void compute(gf_node *node)
{
    /* do nothing */
}

int gf_node_prev(gf_node *node)
{
    int rc;
    int blksize;
    int n;
    gf_cable *c;

    rc = gf_node_cables_alloc(node, 1);
    gf_node_set_block(node, 0);
    if (rc != GF_OK) return rc;

    gf_node_set_compute(node, compute);

    /* zero out cable */

    gf_node_get_cable(node, 0, &c);

    blksize = gf_node_blksize(node);

    for (n = 0; n < blksize; n++) {
        gf_cable_set(c, n, 0.0);
    }

    return GF_OK;
}

int sk_node_prev(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_prev(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_out(core, node, 0);
    return 0;
}
