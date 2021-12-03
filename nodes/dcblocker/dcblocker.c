#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_DCBLOCKER_PRIV
#include "dsp/dcblocker.h"

struct dcblocker_n {
    gf_cable *in;
    gf_cable *out;
    sk_dcblocker dcblocker;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct dcblocker_n *dcblocker;

    blksize = gf_node_blksize(node);

    dcblocker = (struct dcblocker_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, in;

        in = gf_cable_get(dcblocker->in, n);
        out = sk_dcblocker_tick(&dcblocker->dcblocker, in);

        gf_cable_set(dcblocker->out, n, out);
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

int sk_node_dcblocker(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct dcblocker_n *dcblocker;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct dcblocker_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    dcblocker = (struct dcblocker_n *)ud;

    sk_dcblocker_init(&dcblocker->dcblocker);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &dcblocker->in);
    gf_node_get_cable(node, 1, &dcblocker->out);

    gf_node_set_data(node, dcblocker);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}
