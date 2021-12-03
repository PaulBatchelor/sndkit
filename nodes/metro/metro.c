#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_METRO_PRIV
#include "dsp/metro.h"

struct metro_n {
    gf_cable *freq;
    gf_cable *out;
    sk_metro metro;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct metro_n *metro;

    blksize = gf_node_blksize(node);

    metro = (struct metro_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, freq;
        freq = gf_cable_get(metro->freq, n);

        sk_metro_freq(&metro->metro, freq);
        out = sk_metro_tick(&metro->metro);
        gf_cable_set(metro->out, n, out);
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

int sk_node_metro(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct metro_n *metro;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct metro_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    metro = (struct metro_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_metro_init(&metro->metro, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &metro->freq);
    gf_node_get_cable(node, 1, &metro->out);

    gf_node_set_data(node, metro);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}
