#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_RLINE_PRIV
#include "dsp/rline.h"

struct jitseg_n {
    gf_cable *min;
    gf_cable *max;
    gf_cable *minrate;
    gf_cable *maxrate;
    gf_cable *rate;
    gf_cable *out;
    sk_jitseg jitseg;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct jitseg_n *jitseg;

    blksize = gf_node_blksize(node);

    jitseg = (struct jitseg_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT min, max, minrate, maxrate, rate, out;
        min = gf_cable_get(jitseg->min, n);
        max = gf_cable_get(jitseg->max, n);
        minrate = gf_cable_get(jitseg->minrate, n);
        maxrate = gf_cable_get(jitseg->maxrate, n);
        rate = gf_cable_get(jitseg->rate, n);

        sk_jitseg_min(&jitseg->jitseg, min);
        sk_jitseg_max(&jitseg->jitseg, max);
        sk_jitseg_rate_min(&jitseg->jitseg, minrate);
        sk_jitseg_rate_max(&jitseg->jitseg, maxrate);
        sk_jitseg_rate_rate(&jitseg->jitseg, rate);
        out = sk_jitseg_tick(&jitseg->jitseg);
        gf_cable_set(jitseg->out, n, out);
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

int sk_node_jitseg(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param min, max, minrate, maxrate, rate;
    void *ud;
    struct jitseg_n *jitseg;
    int sr;

    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &maxrate);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &minrate);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &max);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &min);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct jitseg_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    jitseg = (struct jitseg_n *)ud;

    sr = gf_patch_srate_get(patch);

    sk_jitseg_init(&jitseg->jitseg,
                   sr,
                   sk_core_rand(core),
                   sk_core_rand(core));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 6);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 5);

    gf_node_get_cable(node, 0, &jitseg->min);
    gf_node_get_cable(node, 1, &jitseg->max);
    gf_node_get_cable(node, 2, &jitseg->minrate);
    gf_node_get_cable(node, 3, &jitseg->maxrate);
    gf_node_get_cable(node, 4, &jitseg->rate);
    gf_node_get_cable(node, 5, &jitseg->out);

    gf_node_set_data(node, jitseg);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &min, 0);
    sk_param_set(core, node, &max, 1);
    sk_param_set(core, node, &minrate, 2);
    sk_param_set(core, node, &maxrate, 3);
    sk_param_set(core, node, &rate, 4);
    sk_param_out(core, node, 5);
    return 0;
}
