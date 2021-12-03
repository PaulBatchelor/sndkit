#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_NOISE_PRIV
#include "dsp/noise.h"

struct noise_n {
    gf_cable *out;
    sk_noise noise;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct noise_n *noise;

    blksize = gf_node_blksize(node);

    noise = (struct noise_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out;
        out = sk_noise_tick(&noise->noise);
        gf_cable_set(noise->out, n, out);
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

int sk_node_noise(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    void *ud;
    struct noise_n *noise;

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct noise_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    noise = (struct noise_n *)ud;

    sk_noise_init(&noise->noise, sk_core_rand(core));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 1);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 0);

    gf_node_get_cable(node, 0, &noise->out);

    gf_node_set_data(node, noise);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_out(core, node, 0);
    return 0;
}
