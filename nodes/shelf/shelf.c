#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_SHELF_PRIV
#include "dsp/shelf.h"

struct shelf_n {
    gf_cable *in;
    gf_cable *freq;
    gf_cable *gain;
    gf_cable *slope;
    gf_cable *out;
    sk_shelf shelf;
};

static void high_compute(gf_node *node)
{
    int blksize;
    int n;
    struct shelf_n *shelf;

    blksize = gf_node_blksize(node);

    shelf = (struct shelf_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, gain, slope, out;

        in = gf_cable_get(shelf->in, n);
        freq = gf_cable_get(shelf->freq, n);
        gain = gf_cable_get(shelf->gain, n);
        slope = gf_cable_get(shelf->slope, n);

        sk_shelf_frequency(&shelf->shelf, freq);
        sk_shelf_gain(&shelf->shelf, gain);
        sk_shelf_slope(&shelf->shelf, slope);

        out = sk_shelf_high_tick(&shelf->shelf, in);
        gf_cable_set(shelf->out, n, out);
    }
}

static void low_compute(gf_node *node)
{
    int blksize;
    int n;
    struct shelf_n *shelf;

    blksize = gf_node_blksize(node);

    shelf = (struct shelf_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, gain, slope, out;

        in = gf_cable_get(shelf->in, n);
        freq = gf_cable_get(shelf->freq, n);
        gain = gf_cable_get(shelf->gain, n);
        slope = gf_cable_get(shelf->slope, n);

        sk_shelf_frequency(&shelf->shelf, freq);
        sk_shelf_gain(&shelf->shelf, gain);
        sk_shelf_slope(&shelf->shelf, slope);

        out = sk_shelf_low_tick(&shelf->shelf, in);
        gf_cable_set(shelf->out, n, out);
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

int sk_node_highshelf(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param freq;
    sk_param gain;
    sk_param slope;
    void *ud;
    struct shelf_n *shelf;
    int sr;

    rc = sk_param_get(core, &slope);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &gain);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct shelf_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    shelf = (struct shelf_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_shelf_init(&shelf->shelf, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &shelf->in);
    gf_node_get_cable(node, 1, &shelf->freq);
    gf_node_get_cable(node, 2, &shelf->gain);
    gf_node_get_cable(node, 3, &shelf->slope);
    gf_node_get_cable(node, 4, &shelf->out);

    gf_node_set_data(node, shelf);
    gf_node_set_compute(node, high_compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &gain, 2);
    sk_param_set(core, node, &slope, 3);
    sk_param_out(core, node, 4);
    return 0;
}

int sk_node_lowshelf(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param freq;
    sk_param gain;
    sk_param slope;
    void *ud;
    struct shelf_n *shelf;
    int sr;

    rc = sk_param_get(core, &slope);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &gain);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct shelf_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    shelf = (struct shelf_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_shelf_init(&shelf->shelf, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &shelf->in);
    gf_node_get_cable(node, 1, &shelf->freq);
    gf_node_get_cable(node, 2, &shelf->gain);
    gf_node_get_cable(node, 3, &shelf->slope);
    gf_node_get_cable(node, 4, &shelf->out);

    gf_node_set_data(node, shelf);
    gf_node_set_compute(node, low_compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &gain, 2);
    sk_param_set(core, node, &slope, 3);
    sk_param_out(core, node, 4);
    return 0;
}
