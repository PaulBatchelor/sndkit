#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TRAND_PRIV
#include "dsp/trand.h"

struct trand_n {
    gf_cable *trig;
    gf_cable *min;
    gf_cable *max;
    gf_cable *out;
    sk_trand trand;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct trand_n *trand;

    blksize = gf_node_blksize(node);

    trand = (struct trand_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, min, max, out;

        trig = gf_cable_get(trand->trig, n);
        min = gf_cable_get(trand->min, n);
        max = gf_cable_get(trand->max, n);

        sk_trand_min(&trand->trand, min);
        sk_trand_max(&trand->trand, max);
        out = sk_trand_tick(&trand->trand, trig);

        gf_cable_set(trand->out, n, out);
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

int sk_node_trand(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param min;
    sk_param max;
    void *ud;
    struct trand_n *trand;

    rc = sk_param_get(core, &max);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &min);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct trand_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    trand = (struct trand_n *)ud;

    sk_trand_init(&trand->trand, sk_core_rand(core));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &trand->trig);
    gf_node_get_cable(node, 1, &trand->min);
    gf_node_get_cable(node, 2, &trand->max);
    gf_node_get_cable(node, 3, &trand->out);

    gf_node_set_data(node, trand);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &min, 1);
    sk_param_set(core, node, &max, 2);
    sk_param_out(core, node, 3);

    return 0;
}
