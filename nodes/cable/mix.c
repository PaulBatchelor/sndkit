/*
 * Mix
 *
 * Takes in a signal, and mixes it into a specific cable.
 * This cable is useful for things like sends and throws.
 */

#include "graforge.h"
#include "core.h"

static void compute(gf_node *node)
{
    int blksize;
    int s;
    gf_cable *sum;
    gf_cable *in;
    gf_cable *mix;
    GFFLT f_mix;
    GFFLT f_in;
    GFFLT f_out;

    blksize = gf_node_blksize(node);
    gf_node_get_cable(node, 0, &in);
    gf_node_get_cable(node, 1, &mix);
    sum = gf_node_get_data(node);

    for(s = 0; s < blksize; s++) {
        f_in = gf_cable_get(in, s);
        f_mix = gf_cable_get(mix, s);
        f_out = gf_cable_get(sum, s);
        gf_cable_set(sum, s, f_out + (f_in * f_mix));
    }
}

int gf_node_mix(gf_node *node, gf_cable *sum)
{
    int rc;

    rc = gf_node_cables_alloc(node, 2);
    if (rc != GF_OK) return rc;
    gf_node_set_compute(node, compute);
    gf_node_set_data(node, sum);

    return GF_OK;
}

int sk_node_mix(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param amt;
    sk_param cable;
    gf_cable *cab;

    rc = sk_param_get(core, &amt);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get_cable(core, &cable);
    SK_ERROR_CHECK(rc);
    cab = sk_param_cable(&cable);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_mix(node, cab);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &amt, 1);
    return 0;
}
