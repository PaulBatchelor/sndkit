#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_VARDELAY_PRIV
#include "dsp/vardelay.h"

struct vardelay_n {
    pw_cable *in;
    pw_cable *feedback;
    pw_cable *delay;
    pw_cable *out;
    sk_vardelay vardelay;
    SKFLT *buf;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct vardelay_n *vardelay;

    blksize = pw_node_blksize(node);

    vardelay = (struct vardelay_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, delay, feedback, out;
        in = pw_cable_get(vardelay->in, n);
        feedback = pw_cable_get(vardelay->feedback, n);
        sk_vardelay_feedback(&vardelay->vardelay, feedback);
        delay = pw_cable_get(vardelay->delay, n);
        sk_vardelay_delay(&vardelay->vardelay, delay);

        out = sk_vardelay_tick(&vardelay->vardelay, in);
        pw_cable_set(vardelay->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    struct vardelay_n *vardelay;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    vardelay = ud;
    ud = vardelay->buf;
    pw_memory_free(patch, &ud);
    ud = vardelay;
    pw_memory_free(patch, &ud);
}

int sk_node_vardelay(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, feedback, delay;
    SKFLT maxdelay;
    void *ud;
    struct vardelay_n *vardelay;
    int sr;
    unsigned int sz;

    rc = sk_param_get_constant(core, &maxdelay);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &delay);
    SK_ERROR_CHECK(rc);
    
    rc = sk_param_get(core, &feedback);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct vardelay_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    vardelay = (struct vardelay_n *)ud;

    sr = pw_patch_srate_get(patch);

    sz = floor(sr * maxdelay);
    rc = pw_memory_alloc(patch, sz * sizeof(SKFLT), &ud);
    SK_PW_ERROR_CHECK(rc);
    vardelay->buf = ud;

    sk_vardelay_init(&vardelay->vardelay, sr,
                     vardelay->buf, sz);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &vardelay->in);
    pw_node_get_cable(node, 1, &vardelay->feedback);
    pw_node_get_cable(node, 2, &vardelay->delay);
    pw_node_get_cable(node, 3, &vardelay->out);

    pw_node_set_data(node, vardelay);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &feedback, 1);
    sk_param_set(core, node, &delay, 2);
    sk_param_out(core, node, 3);
    return 0;
}
