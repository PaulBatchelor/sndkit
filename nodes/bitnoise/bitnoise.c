#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_BITNOISE_PRIV
#include "dsp/bitnoise.h"

struct bitnoise_n {
    gf_cable *rate;
    gf_cable *mode;
    gf_cable *out;
    sk_bitnoise bitnoise;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct bitnoise_n *bitnoise;

    blksize = gf_node_blksize(node);

    bitnoise = (struct bitnoise_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT rate, mode, out;

        rate = gf_cable_get(bitnoise->rate, n);
        sk_bitnoise_rate(&bitnoise->bitnoise, rate);
        mode = gf_cable_get(bitnoise->mode, n);
        sk_bitnoise_mode(&bitnoise->bitnoise, mode);

        out = sk_bitnoise_tick(&bitnoise->bitnoise);
        gf_cable_set(bitnoise->out, n, out);
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

int sk_node_bitnoise(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param rate, mode;
    void *ud;
    struct bitnoise_n *bitnoise;
    int sr;

    rc = sk_param_get(core, &mode);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct bitnoise_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    bitnoise = (struct bitnoise_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_bitnoise_init(&bitnoise->bitnoise, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &bitnoise->rate);
    gf_node_get_cable(node, 1, &bitnoise->mode);
    gf_node_get_cable(node, 2, &bitnoise->out);

    gf_node_set_data(node, bitnoise);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &rate, 0);
    sk_param_set(core, node, &mode, 1);
    sk_param_out(core, node, 2);
    return 0;
}
