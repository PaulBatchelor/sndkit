#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_MTOF_PRIV
#include "dsp/mtof.h"

struct mtof_n {
    pw_cable *nn;
    pw_cable *out;
    sk_mtof mtof;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct mtof_n *mtof;

    blksize = pw_node_blksize(node);

    mtof = (struct mtof_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, nn;
        nn = pw_cable_get(mtof->nn, n);

        out = sk_mtof_tick(&mtof->mtof, nn);
        pw_cable_set(mtof->out, n, out);
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

int sk_node_mtof(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param nn;
    void *ud;
    struct mtof_n *mtof;

    rc = sk_param_get(core, &nn);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct mtof_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    mtof = (struct mtof_n *)ud;

    sk_mtof_init(&mtof->mtof);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &mtof->nn);
    pw_node_get_cable(node, 1, &mtof->out);

    pw_node_set_data(node, mtof);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &nn, 0);
    sk_param_out(core, node, 1);
    return 0;
}
