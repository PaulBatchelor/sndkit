#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TDIV_PRIV
#include "dsp/tdiv.h"

struct tdiv_n {
    gf_cable *trig;
    gf_cable *div;
    gf_cable *off;
    gf_cable *out;
    sk_tdiv tdiv;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tdiv_n *tdiv;

    blksize = gf_node_blksize(node);

    tdiv = (struct tdiv_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, div, off, out;

        trig = gf_cable_get(tdiv->trig, n);
        div = gf_cable_get(tdiv->div, n);
        off = gf_cable_get(tdiv->off, n);

        sk_tdiv_offset(&tdiv->tdiv, (int)off);
        sk_tdiv_divide(&tdiv->tdiv, (int)div);

        out = sk_tdiv_tick(&tdiv->tdiv, trig);

        gf_cable_set(tdiv->out, n, out);
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

int sk_node_tdiv(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig, div, off;
    void *ud;
    struct tdiv_n *tdiv;

    rc = sk_param_get(core, &off);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &div);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tdiv_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tdiv = (struct tdiv_n *)ud;

    sk_tdiv_init(&tdiv->tdiv);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &tdiv->trig);
    gf_node_get_cable(node, 1, &tdiv->div);
    gf_node_get_cable(node, 2, &tdiv->off);
    gf_node_get_cable(node, 3, &tdiv->out);

    gf_node_set_data(node, tdiv);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &div, 1);
    sk_param_set(core, node, &off, 2);
    sk_param_out(core, node, 3);
    return 0;
}
