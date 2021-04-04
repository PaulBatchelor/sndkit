#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_BITNOISE_PRIV
#include "dsp/bitnoise.h"

struct bitnoise_n {
    pw_cable *rate;
    pw_cable *mode;
    pw_cable *out;
    sk_bitnoise bitnoise;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct bitnoise_n *bitnoise;

    blksize = pw_node_blksize(node);

    bitnoise = (struct bitnoise_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT rate, mode, out;

        rate = pw_cable_get(bitnoise->rate, n);
        sk_bitnoise_rate(&bitnoise->bitnoise, rate);
        mode = pw_cable_get(bitnoise->mode, n);
        sk_bitnoise_mode(&bitnoise->bitnoise, mode);

        out = sk_bitnoise_tick(&bitnoise->bitnoise);
        pw_cable_set(bitnoise->out, n, out);
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

int sk_node_bitnoise(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct bitnoise_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    bitnoise = (struct bitnoise_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_bitnoise_init(&bitnoise->bitnoise, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &bitnoise->rate);
    pw_node_get_cable(node, 1, &bitnoise->mode);
    pw_node_get_cable(node, 2, &bitnoise->out);

    pw_node_set_data(node, bitnoise);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &rate, 0);
    sk_param_set(core, node, &mode, 1);
    sk_param_out(core, node, 2);
    return 0;
}
