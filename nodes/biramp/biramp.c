#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#include "dsp/biramp.h"

struct biramp_n {
    gf_cable *in;
    gf_cable *pos;
    gf_cable *out;
    SKFLT lphs;
    SKFLT lpos; /* used for biramp */
    int dir; /* used for flipper */
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct biramp_n *biramp;

    blksize = gf_node_blksize(node);

    biramp = (struct biramp_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, pos, out;
        in = gf_cable_get(biramp->in, n);
        pos = gf_cable_get(biramp->pos, n);

        out = sk_biramp_stateful(in, pos,
                                 &biramp->lphs,
                                 &biramp->lpos);
        gf_cable_set(biramp->out, n, out);
    }
}

static void flipper(gf_node *node)
{
    int blksize;
    int n;
    struct biramp_n *biramp;

    blksize = gf_node_blksize(node);

    biramp = (struct biramp_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, out;
        in = gf_cable_get(biramp->in, n);

        out = sk_biramp_flipper(in, &biramp->lphs, &biramp->dir);
        gf_cable_set(biramp->out, n, out);
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

int sk_node_biramp(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, pos;
    void *ud;
    struct biramp_n *biramp;

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct biramp_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    biramp = (struct biramp_n *)ud;

    biramp->lpos = -1;
    biramp->lphs = -1;
    biramp->dir = 0; /* unused for biramp */

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &biramp->in);
    gf_node_get_cable(node, 1, &biramp->pos);
    gf_node_get_cable(node, 2, &biramp->out);

    gf_node_set_data(node, biramp);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &pos, 1);
    sk_param_out(core, node, 2);
    return 0;
}

int sk_node_flipper(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct biramp_n *biramp;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct biramp_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    biramp = (struct biramp_n *)ud;

    biramp->lpos = -1; /* unused for flipper */
    biramp->lphs = -1;
    biramp->dir = 0;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &biramp->in);
    gf_node_get_cable(node, 1, &biramp->out);

    gf_node_set_data(node, biramp);
    gf_node_set_compute(node, flipper);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}
