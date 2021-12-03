#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#include "dsp/bitosc.h"

static void compute(gf_node *node)
{
    int blksize;
    gf_cable *c[4];
    int n;

    blksize = gf_node_blksize(node);

    for (n = 0; n < 4; n++) {
        gf_node_get_cable(node, n, &c[n]);
    }

    for (n = 0; n < blksize; n++) {
        unsigned long wt;
        int sz;
        GFFLT phs;
        GFFLT out;

        phs = gf_cable_get(c[0], n);
        wt = floor(gf_cable_get(c[1], n));
        sz = floor(gf_cable_get(c[2], n));

        out = sk_bitosc_tick(phs, wt, sz);

        gf_cable_set(c[3], n, out);
    }
}

int node_bitosc(gf_node *node)
{
    gf_node_cables_alloc(node, 4);
    gf_node_set_block(node, 3);
    gf_node_set_compute(node, compute);
    return GF_OK;
}

int sk_node_bitosc(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param pos, wt, sz;

    rc = sk_param_get(core, &sz);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &wt);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = node_bitosc(node);
    SK_GF_ERROR_CHECK(rc);


    sk_param_set(core, node, &pos, 0);
    sk_param_set(core, node, &wt, 1);
    sk_param_set(core, node, &sz, 2);
    sk_param_out(core, node, 3);
    return 0;
}
