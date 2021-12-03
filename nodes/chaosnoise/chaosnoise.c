#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_CHAOSNOISE_PRIV
#include "dsp/chaosnoise.h"

struct chaosnoise_n {
    gf_cable *chaos;
    gf_cable *rate;
    gf_cable *out;
    sk_chaosnoise chaosnoise;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct chaosnoise_n *chaosnoise;

    blksize = gf_node_blksize(node);

    chaosnoise = (struct chaosnoise_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT chaos, rate, out;

        chaos = gf_cable_get(chaosnoise->chaos, n);
        sk_chaosnoise_chaos(&chaosnoise->chaosnoise, chaos);
        rate = gf_cable_get(chaosnoise->rate, n);
        sk_chaosnoise_rate(&chaosnoise->chaosnoise, rate);

        out = sk_chaosnoise_tick(&chaosnoise->chaosnoise);
        gf_cable_set(chaosnoise->out, n, out);
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

int sk_node_chaosnoise(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param chaos, rate;
    SKFLT init;
    void *ud;
    struct chaosnoise_n *chaosnoise;
    int sr;

    rc = sk_param_get_constant(core, &init);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &chaos);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct chaosnoise_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    chaosnoise = (struct chaosnoise_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_chaosnoise_init(&chaosnoise->chaosnoise, sr, init);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &chaosnoise->chaos);
    gf_node_get_cable(node, 1, &chaosnoise->rate);
    gf_node_get_cable(node, 2, &chaosnoise->out);

    gf_node_set_data(node, chaosnoise);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &chaos, 0);
    sk_param_set(core, node, &rate, 1);
    sk_param_out(core, node, 2);
    return 0;
}
