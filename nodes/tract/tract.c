#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"

#define SK_TRACT_PRIV
#include "dsp/tract.h"

struct tract_n {
    pw_cable *in;
    pw_cable *tongue_x;
    pw_cable *tongue_y;
    pw_cable *out;
    sk_tract tract;
};

static void computexy(pw_node *node)
{
    int blksize;
    int n;
    struct tract_n *tract;

    blksize = pw_node_blksize(node);

    tract = (struct tract_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, tongue_x, tongue_y, out;

        in = pw_cable_get(tract->in, n);
        tongue_x = pw_cable_get(tract->tongue_x, n);
        tongue_y = pw_cable_get(tract->tongue_y, n);

        sk_tract_tongue_shape(&tract->tract, tongue_x, tongue_y);

        out = sk_tract_tick(&tract->tract, in);
        pw_cable_set(tract->out, n, out);
    }
}

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct tract_n *tract;

    blksize = pw_node_blksize(node);

    tract = (struct tract_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, out;
        in = pw_cable_get(tract->in, n);
        out = sk_tract_tick(&tract->tract, in);
        pw_cable_set(tract->out, n, out);
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

int sk_node_tractxy(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in;
    sk_param tx;
    sk_param ty;
    void *ud;
    struct tract_n *tract;

    rc = sk_param_get(core, &ty);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &tx);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct tract_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    tract = (struct tract_n *)ud;

    sk_tract_init(&tract->tract);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &tract->in);
    pw_node_get_cable(node, 1, &tract->tongue_x);
    pw_node_get_cable(node, 2, &tract->tongue_y);
    pw_node_get_cable(node, 3, &tract->out);

    pw_node_set_data(node, tract);
    pw_node_set_compute(node, computexy);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &tx, 1);
    sk_param_set(core, node, &ty, 2);
    sk_param_out(core, node, 3);
    return 0;
}

sk_tract * sk_node_tract_data(pw_node *node)
{
    struct tract_n *tract;
    tract = (struct tract_n *)pw_node_get_data(node);
    return &tract->tract;
}

int sk_node_tract(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct tract_n *tract;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct tract_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    tract = (struct tract_n *)ud;

    sk_tract_init(&tract->tract);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &tract->in);
    pw_node_get_cable(node, 1, &tract->out);

    pw_node_set_data(node, tract);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}
