#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TGATE_PRIV
#include "dsp/tgate.h"

struct tgate_n {
    gf_cable *trig;
    gf_cable *dur;
    gf_cable *out;
    sk_tgate tgate;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tgate_n *tgate;

    blksize = gf_node_blksize(node);

    tgate = (struct tgate_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, dur, out;
        trig = gf_cable_get(tgate->trig, n);
        dur = gf_cable_get(tgate->dur, n);

        sk_tgate_dur(&tgate->tgate, dur);
        out = sk_tgate_tick(&tgate->tgate, trig);
        gf_cable_set(tgate->out, n, out);
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

int sk_node_tgate(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param dur;
    void *ud;
    struct tgate_n *tgate;
    int sr;

    rc = sk_param_get(core, &dur);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tgate_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tgate = (struct tgate_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_tgate_init(&tgate->tgate, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &tgate->trig);
    gf_node_get_cable(node, 1, &tgate->dur);
    gf_node_get_cable(node, 2, &tgate->out);

    gf_node_set_data(node, tgate);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &dur, 1);
    sk_param_out(core, node, 2);
    return 0;
}
