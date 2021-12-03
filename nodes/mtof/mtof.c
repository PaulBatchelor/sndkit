#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_MTOF_PRIV
#include "dsp/mtof.h"

struct mtof_n {
    gf_cable *nn;
    gf_cable *out;
    sk_mtof mtof;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct mtof_n *mtof;

    blksize = gf_node_blksize(node);

    mtof = (struct mtof_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, nn;
        nn = gf_cable_get(mtof->nn, n);

        out = sk_mtof_tick(&mtof->mtof, nn);
        gf_cable_set(mtof->out, n, out);
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

int sk_node_mtof(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param nn;
    void *ud;
    struct mtof_n *mtof;

    rc = sk_param_get(core, &nn);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct mtof_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    mtof = (struct mtof_n *)ud;

    sk_mtof_init(&mtof->mtof);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &mtof->nn);
    gf_node_get_cable(node, 1, &mtof->out);

    gf_node_set_data(node, mtof);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &nn, 0);
    sk_param_out(core, node, 1);
    return 0;
}
