#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_SMOOTHER_PRIV
#include "dsp/smoother.h"

struct smoother_n {
    pw_cable *in;
    pw_cable *smooth;
    pw_cable *out;
    sk_smoother smoother;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct smoother_n *smoother;

    blksize = pw_node_blksize(node);

    smoother = (struct smoother_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, smooth, in;
        in = pw_cable_get(smoother->in, n);
        smooth = pw_cable_get(smoother->smooth, n);

        sk_smoother_time(&smoother->smoother, smooth);
        out = sk_smoother_tick(&smoother->smoother, in);

        pw_cable_set(smoother->out, n, out);
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

int sk_node_smoother(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in;
    sk_param smooth;
    void *ud;
    struct smoother_n *smoother;
    int sr;

    rc = sk_param_get(core, &smooth);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct smoother_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    smoother = (struct smoother_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_smoother_init(&smoother->smoother, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &smoother->in);
    pw_node_get_cable(node, 1, &smoother->smooth);
    pw_node_get_cable(node, 2, &smoother->out);

    pw_node_set_data(node, smoother);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &smooth, 1);
    sk_param_out(core, node, 2);
    return 0;
}
