#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#include "dsp/phasewarp.h"

struct phasewarp_n {
    gf_cable *in;
    gf_cable *pos;
    gf_cable *out;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct phasewarp_n *phasewarp;

    blksize = gf_node_blksize(node);

    phasewarp = (struct phasewarp_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, pos, out;
        in = gf_cable_get(phasewarp->in, n);
        pos = gf_cable_get(phasewarp->pos, n);

        out = sk_phasewarp_tick(in, pos);
        gf_cable_set(phasewarp->out, n, out);
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

int sk_node_phasewarp(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, pos;
    void *ud;
    struct phasewarp_n *phasewarp;

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct phasewarp_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    phasewarp = (struct phasewarp_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &phasewarp->in);
    gf_node_get_cable(node, 1, &phasewarp->pos);
    gf_node_get_cable(node, 2, &phasewarp->out);

    gf_node_set_data(node, phasewarp);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &pos, 1);
    sk_param_out(core, node, 2);
    return 0;
}
