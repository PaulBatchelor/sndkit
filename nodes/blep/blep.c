#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

#define SK_BLEP_PRIV
#include "dsp/blep.h"

#define SK_BLEP_PRIV

struct blep_n {
    gf_cable *freq;
    gf_cable *out;
    sk_blep blep;
};

static void blep_compute(gf_node *node, SKFLT (*tick)(sk_blep *))
{
    int blksize;
    int n;
    struct blep_n *blep;

    blksize = gf_node_blksize(node);

    blep = (struct blep_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, freq;
        freq = gf_cable_get(blep->freq, n);

        sk_blep_freq(&blep->blep, freq);
        out = tick(&blep->blep);
        gf_cable_set(blep->out, n, out);
    }
}

static void compute_saw(gf_node *node)
{
    blep_compute(node, sk_blep_saw);
}

static void compute_square(gf_node *node)
{
    blep_compute(node, sk_blep_square);
}

static void compute_triangle(gf_node *node)
{
    blep_compute(node, sk_blep_triangle);
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

static int node_blep(sk_core *core, gf_function compute)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct blep_n *blep;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct blep_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    blep = (struct blep_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_blep_init(&blep->blep, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &blep->freq);
    gf_node_get_cable(node, 1, &blep->out);

    gf_node_set_data(node, blep);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}

int sk_node_blsaw(sk_core *core)
{
    return node_blep(core, compute_saw);
}

int sk_node_blsquare(sk_core *core)
{
    return node_blep(core, compute_square);
}

int sk_node_bltri(sk_core *core)
{
    return node_blep(core, compute_triangle);
}
