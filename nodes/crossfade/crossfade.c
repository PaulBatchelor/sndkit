#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#include "dsp/crossfade.h"

struct crossfade_n {
    gf_cable *a;
    gf_cable *b;
    gf_cable *pos;
    gf_cable *out;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct crossfade_n *crossfade;

    blksize = gf_node_blksize(node);

    crossfade = (struct crossfade_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT a, b, pos, out;

        a = gf_cable_get(crossfade->a, n);
        b = gf_cable_get(crossfade->b, n);
        pos = gf_cable_get(crossfade->pos, n);

        out = sk_crossfade_linear(a, b, pos);
        gf_cable_set(crossfade->out, n, out);
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

int sk_node_crossfade(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param a;
    sk_param b;
    sk_param pos;
    void *ud;
    struct crossfade_n *crossfade;

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &b);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &a);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct crossfade_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    crossfade = (struct crossfade_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &crossfade->a);
    gf_node_get_cable(node, 1, &crossfade->b);
    gf_node_get_cable(node, 2, &crossfade->pos);
    gf_node_get_cable(node, 3, &crossfade->out);

    gf_node_set_data(node, crossfade);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &a, 0);
    sk_param_set(core, node, &b, 1);
    sk_param_set(core, node, &pos, 2);
    sk_param_out(core, node, 3);
    return 0;
}
