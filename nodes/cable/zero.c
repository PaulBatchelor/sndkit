/*
 * Zero
 *
 * Creates an audio-rate signal that puts out 0.
 * A good way to initialize a send/throw cable.
 *
 */

#include "graforge.h"
#include "core.h"

static void compute(gf_node *node)
{
    int blksize;
    int s;
    gf_cable *out;

    blksize = gf_node_blksize(node);
    gf_node_get_cable(node, 0, &out);

    for(s = 0; s < blksize; s++) {
        gf_cable_set(out, s, 0);
    }
}

int gf_node_zero(gf_node *node)
{
    int rc;

    rc = gf_node_cables_alloc(node, 1);
    gf_node_set_block(node, 0);
    if (rc != GF_OK) return rc;
    gf_node_set_compute(node, compute);

    return GF_OK;
}

int sk_node_zero(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_zero(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_out(core, node, 0);
    return 0;
}
