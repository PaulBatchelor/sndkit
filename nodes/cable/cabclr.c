/*
 * cabclr
 *
 * Zeros out an input cable.
 *
 */

#include "graforge.h"
#include "core.h"

static void compute(gf_node *node)
{
    int blksize;
    int s;
    gf_cable *cab;

    blksize = gf_node_blksize(node);
    cab = gf_node_get_data(node);

    for(s = 0; s < blksize; s++) {
        gf_cable_set(cab, s, 0.0);
    }
}

int gf_node_cabclr(gf_node *node, gf_cable *cab)
{
    gf_node_set_compute(node, compute);
    gf_node_set_data(node, cab);
    return GF_OK;
}

int sk_node_cabclr(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param cable;
    gf_cable *cab;

    rc = sk_param_get_cable(core, &cable);
    SK_ERROR_CHECK(rc);
    cab = sk_param_cable(&cable);

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cabclr(node, cab);
    SK_GF_ERROR_CHECK(rc);

    return 0;
}
