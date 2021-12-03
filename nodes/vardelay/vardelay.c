#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_VARDELAY_PRIV
#include "dsp/vardelay.h"

struct vardelay_n {
    gf_cable *in;
    gf_cable *feedback;
    gf_cable *delay;
    gf_cable *out;
    sk_vardelay vardelay;
    SKFLT *buf;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct vardelay_n *vardelay;

    blksize = gf_node_blksize(node);

    vardelay = (struct vardelay_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, delay, feedback, out;
        in = gf_cable_get(vardelay->in, n);
        feedback = gf_cable_get(vardelay->feedback, n);
        sk_vardelay_feedback(&vardelay->vardelay, feedback);
        delay = gf_cable_get(vardelay->delay, n);
        sk_vardelay_delay(&vardelay->vardelay, delay);

        out = sk_vardelay_tick(&vardelay->vardelay, in);
        gf_cable_set(vardelay->out, n, out);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct vardelay_n *vardelay;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    vardelay = ud;
    ud = vardelay->buf;
    gf_memory_free(patch, &ud);
    ud = vardelay;
    gf_memory_free(patch, &ud);
}

int sk_node_vardelay(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, feedback, delay;
    SKFLT maxdelay;
    void *ud;
    struct vardelay_n *vardelay;
    int sr;
    unsigned int sz;
    unsigned int i;

    rc = sk_param_get_constant(core, &maxdelay);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &delay);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &feedback);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct vardelay_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    vardelay = (struct vardelay_n *)ud;

    sr = gf_patch_srate_get(patch);

    sz = floor(sr * maxdelay);
    rc = gf_memory_alloc(patch, sz * sizeof(SKFLT), &ud);
    SK_GF_ERROR_CHECK(rc);
    vardelay->buf = ud;

    for (i = 0; i < sz; i++) vardelay->buf[i] = 0;

    sk_vardelay_init(&vardelay->vardelay, sr,
                     vardelay->buf, sz);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &vardelay->in);
    gf_node_get_cable(node, 1, &vardelay->feedback);
    gf_node_get_cable(node, 2, &vardelay->delay);
    gf_node_get_cable(node, 3, &vardelay->out);

    gf_node_set_data(node, vardelay);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &feedback, 1);
    sk_param_set(core, node, &delay, 2);
    sk_param_out(core, node, 3);
    return 0;
}
