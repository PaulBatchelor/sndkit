#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_GTICK_PRIV
#include "dsp/gtick.h"

struct gtick_n {
    gf_cable *gate;
    gf_cable *out;
    sk_gtick gtick;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct gtick_n *gtick;

    blksize = gf_node_blksize(node);

    gtick = (struct gtick_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, gate;
        gate = gf_cable_get(gtick->gate, n);
        out = sk_gtick_tick(&gtick->gtick, gate);
        gf_cable_set(gtick->out, n, out);
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

int sk_node_gtick(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param gate;
    void *ud;
    struct gtick_n *gtick;

    rc = sk_param_get(core, &gate);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct gtick_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    gtick = (struct gtick_n *)ud;

    sk_gtick_init(&gtick->gtick);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &gtick->gate);
    gf_node_get_cable(node, 1, &gtick->out);

    gf_node_set_data(node, gtick);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &gate, 0);
    sk_param_out(core, node, 1);
    return 0;
}
