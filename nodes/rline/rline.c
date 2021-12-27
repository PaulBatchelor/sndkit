#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_RLINE_PRIV
#include "dsp/rline.h"

struct rline_n {
    gf_cable *min;
    gf_cable *max;
    gf_cable *rate;
    gf_cable *out;
    sk_rline rline;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct rline_n *rline;

    blksize = gf_node_blksize(node);

    rline = (struct rline_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT min, max, rate, out;
        min = gf_cable_get(rline->min, n);
        max = gf_cable_get(rline->max, n);
        rate = gf_cable_get(rline->rate, n);

        sk_rline_min(&rline->rline, min);
        sk_rline_max(&rline->rline, max);
        sk_rline_rate(&rline->rline, rate);
        out = sk_rline_tick(&rline->rline);
        gf_cable_set(rline->out, n, out);
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

int sk_node_rline(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param min, max, rate;
    void *ud;
    struct rline_n *rline;
    int sr;

    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &max);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &min);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct rline_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    rline = (struct rline_n *)ud;

    sr = gf_patch_srate_get(patch);

    sk_rline_init(&rline->rline, sr, sk_core_rand(core));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &rline->min);
    gf_node_get_cable(node, 1, &rline->max);
    gf_node_get_cable(node, 2, &rline->rate);
    gf_node_get_cable(node, 3, &rline->out);

    gf_node_set_data(node, rline);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &min, 0);
    sk_param_set(core, node, &max, 1);
    sk_param_set(core, node, &rate, 2);
    sk_param_out(core, node, 3);
    return 0;
}
