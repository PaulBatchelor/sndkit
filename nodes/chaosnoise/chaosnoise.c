#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_CHAOSNOISE_PRIV
#include "dsp/chaosnoise.h"

struct chaosnoise_n {
    pw_cable *chaos;
    pw_cable *rate;
    pw_cable *out;
    sk_chaosnoise chaosnoise;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct chaosnoise_n *chaosnoise;

    blksize = pw_node_blksize(node);

    chaosnoise = (struct chaosnoise_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT chaos, rate, out;

        chaos = pw_cable_get(chaosnoise->chaos, n);
        sk_chaosnoise_chaos(&chaosnoise->chaosnoise, chaos);
        rate = pw_cable_get(chaosnoise->rate, n);
        sk_chaosnoise_rate(&chaosnoise->chaosnoise, rate);

        out = sk_chaosnoise_tick(&chaosnoise->chaosnoise);
        pw_cable_set(chaosnoise->out, n, out);
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

int sk_node_chaosnoise(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct chaosnoise_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    chaosnoise = (struct chaosnoise_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_chaosnoise_init(&chaosnoise->chaosnoise, sr, init);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &chaosnoise->chaos);
    pw_node_get_cable(node, 1, &chaosnoise->rate);
    pw_node_get_cable(node, 2, &chaosnoise->out);

    pw_node_set_data(node, chaosnoise);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &chaos, 0);
    sk_param_set(core, node, &rate, 1);
    sk_param_out(core, node, 2);
    return 0;
}
