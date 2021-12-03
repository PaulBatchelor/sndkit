#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

#define SK_TRACT_PRIV
#include "dsp/tract.h"

struct tract_n {
    gf_cable *in;
    gf_cable *tongue_x;
    gf_cable *tongue_y;
    gf_cable *out;
    sk_tract *tract;
};

sk_tract * sk_node_tractnew(sk_core *core);

static void computexy(gf_node *node)
{
    int blksize;
    int n;
    struct tract_n *tract;

    blksize = gf_node_blksize(node);

    tract = (struct tract_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, tongue_x, tongue_y, out;

        in = gf_cable_get(tract->in, n);
        tongue_x = gf_cable_get(tract->tongue_x, n);
        tongue_y = gf_cable_get(tract->tongue_y, n);

        sk_tract_tongue_shape(tract->tract, tongue_x, tongue_y);

        out = sk_tract_tick(tract->tract, in);
        gf_cable_set(tract->out, n, out);
    }
}

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tract_n *tract;

    blksize = gf_node_blksize(node);

    tract = (struct tract_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, out;
        in = gf_cable_get(tract->in, n);
        out = sk_tract_tick(tract->tract, in);
        gf_cable_set(tract->out, n, out);
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

int sk_node_tractxy(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
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

    rc = gf_memory_alloc(patch, sizeof(struct tract_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tract = (struct tract_n *)ud;

    tract->tract = sk_node_tractnew(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &tract->in);
    gf_node_get_cable(node, 1, &tract->tongue_x);
    gf_node_get_cable(node, 2, &tract->tongue_y);
    gf_node_get_cable(node, 3, &tract->out);

    gf_node_set_data(node, tract);
    gf_node_set_compute(node, computexy);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &tx, 1);
    sk_param_set(core, node, &ty, 2);
    sk_param_out(core, node, 3);
    return 0;
}

sk_tract * sk_node_tract_data(gf_node *node)
{
    struct tract_n *tract;
    tract = (struct tract_n *)gf_node_get_data(node);
    return tract->tract;
}

int gf_node_tract(gf_node *node, sk_tract *tr)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct tract_n *tract;

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct tract_n), &ud);
    if (rc != GF_OK) return rc;
    tract = (struct tract_n *)ud;

    tract->tract = tr;

    rc = gf_node_cables_alloc(node, 2);
    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &tract->in);
    gf_node_get_cable(node, 1, &tract->out);

    gf_node_set_data(node, tract);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);
    return GF_OK;
}

int sk_node_tract(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_tract *tr;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    tr = sk_node_tractnew(core);

    rc = gf_node_tract(node, tr);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}

static void cleanup_tract(gf_pointer *p)
{
    sk_tract *tract;

    tract = (sk_tract *)gf_pointer_data(p);

    free(tract);
}

sk_tract * gf_node_tractnew(gf_patch *patch)
{
    sk_tract *tract;

    tract = malloc(sizeof(sk_tract));

    sk_tract_init(tract);
    gf_patch_append_userdata(patch, cleanup_tract, tract);

    return tract;
}

sk_tract * sk_node_tractnew(sk_core *core)
{
    gf_patch *patch;
    patch = sk_core_patch(core);

    return gf_node_tractnew(patch);
}
