#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_RLINE_PRIV
#include "dsp/rline.h"

struct rline_n {
    pw_cable *min;
    pw_cable *max;
    pw_cable *rate;
    pw_cable *out;
    sk_rline rline;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct rline_n *rline;

    blksize = pw_node_blksize(node);

    rline = (struct rline_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT min, max, rate, out;
        min = pw_cable_get(rline->min, n);
        max = pw_cable_get(rline->max, n);
        rate = pw_cable_get(rline->rate, n);

        sk_rline_min(&rline->rline, min);
        sk_rline_max(&rline->rline, max);
        sk_rline_rate(&rline->rline, rate);
        out = sk_rline_tick(&rline->rline);
        pw_cable_set(rline->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    pw_memory_free(patch, &ud);
}

int sk_node_rline(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct rline_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    rline = (struct rline_n *)ud;

    sr = pw_patch_srate_get(patch);

    /* create initial seed using system RNG */
    sk_rline_init(&rline->rline, sr, rand());

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &rline->min);
    pw_node_get_cable(node, 1, &rline->max);
    pw_node_get_cable(node, 2, &rline->rate);
    pw_node_get_cable(node, 3, &rline->out);

    pw_node_set_data(node, rline);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &min, 0);
    sk_param_set(core, node, &max, 1);
    sk_param_set(core, node, &rate, 2);
    sk_param_out(core, node, 3);
    return 0;
}
