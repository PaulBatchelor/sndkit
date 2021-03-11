#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_DCBLOCKER_PRIV
#include "dsp/dcblocker.h"

struct dcblocker_n {
    pw_cable *in;
    pw_cable *out;
    sk_dcblocker dcblocker;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct dcblocker_n *dcblocker;

    blksize = pw_node_blksize(node);

    dcblocker = (struct dcblocker_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, in;

        in = pw_cable_get(dcblocker->in, n);
        out = sk_dcblocker_tick(&dcblocker->dcblocker, in);

        pw_cable_set(dcblocker->out, n, out);
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

int sk_node_dcblocker(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct dcblocker_n *dcblocker;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct dcblocker_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    dcblocker = (struct dcblocker_n *)ud;

    sk_dcblocker_init(&dcblocker->dcblocker);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &dcblocker->in);
    pw_node_get_cable(node, 1, &dcblocker->out);

    pw_node_set_data(node, dcblocker);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}
