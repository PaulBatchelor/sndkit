#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

struct tick_n {
    SKFLT tick;
    gf_cable *out;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tick_n *tick;

    blksize = gf_node_blksize(node);

    tick = (struct tick_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        SKFLT out;
        out = tick->tick;
        if (tick->tick) tick->tick = 0;
        gf_cable_set(tick->out, n, out);
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

int sk_node_tick(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    void *ud;
    struct tick_n *tick;

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tick_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tick = (struct tick_n *)ud;

    tick->tick = 1.0;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 1);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 0);

    gf_node_get_cable(node, 0, &tick->out);

    gf_node_set_data(node, tick);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_out(core, node, 0);
    return 0;
}
