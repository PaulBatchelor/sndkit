#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_REPHASOR_PRIV
#include "dsp/rephasor.h"

struct rephasor_n {
    gf_cable *ext;
    gf_cable *scale;
    gf_cable *out;
    sk_rephasor rephasor;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct rephasor_n *rephasor;

    blksize = gf_node_blksize(node);

    rephasor = (struct rephasor_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, ext, scale;
        ext = gf_cable_get(rephasor->ext, n);
        scale = gf_cable_get(rephasor->scale, n);

        sk_rephasor_scale(&rephasor->rephasor, scale);
        out = sk_rephasor_tick(&rephasor->rephasor, ext);

        gf_cable_set(rephasor->out, n, out);
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

int sk_node_rephasor(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param ext;
    sk_param scale;
    void *ud;
    struct rephasor_n *rephasor;

    rc = sk_param_get(core, &scale);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &ext);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct rephasor_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    rephasor = (struct rephasor_n *)ud;

    sk_rephasor_init(&rephasor->rephasor);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &rephasor->ext);
    gf_node_get_cable(node, 1, &rephasor->scale);
    gf_node_get_cable(node, 2, &rephasor->out);

    gf_node_set_data(node, rephasor);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &ext, 0);
    sk_param_set(core, node, &scale, 1);
    sk_param_out(core, node, 2);
    return 0;
}
