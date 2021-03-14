#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_METRO_PRIV
#include "dsp/metro.h"

struct metro_n {
    pw_cable *freq;
    pw_cable *out;
    sk_metro metro;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct metro_n *metro;

    blksize = pw_node_blksize(node);

    metro = (struct metro_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, freq;
        freq = pw_cable_get(metro->freq, n);

        sk_metro_freq(&metro->metro, freq);
        out = sk_metro_tick(&metro->metro);
        pw_cable_set(metro->out, n, out);
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

int sk_node_metro(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct metro_n *metro;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct metro_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    metro = (struct metro_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_metro_init(&metro->metro, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &metro->freq);
    pw_node_get_cable(node, 1, &metro->out);

    pw_node_set_data(node, metro);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}
