#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"

#define SK_BLEP_PRIV
#include "dsp/blep.h"

#define SK_BLEP_PRIV
#include "blep.h"

struct blep_n {
    pw_cable *freq;
    pw_cable *out;
    sk_blep blep;
};

static void blep_compute(pw_node *node, SKFLT (*tick)(sk_blep *))
{
    int blksize;
    int n;
    struct blep_n *blep;

    blksize = pw_node_blksize(node);

    blep = (struct blep_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, freq;
        freq = pw_cable_get(blep->freq, n);

        sk_blep_freq(&blep->blep, freq);
        out = tick(&blep->blep);
        pw_cable_set(blep->out, n, out);
    }
}

static void compute_saw(pw_node *node)
{
    blep_compute(node, sk_blep_saw);
}

static void compute_square(pw_node *node)
{
    blep_compute(node, sk_blep_square);
}

static void compute_triangle(pw_node *node)
{
    blep_compute(node, sk_blep_triangle);
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

static int node_blep(sk_core *core, pw_function compute)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct blep_n *blep;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct blep_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    blep = (struct blep_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_blep_init(&blep->blep, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &blep->freq);
    pw_node_get_cable(node, 1, &blep->out);

    pw_node_set_data(node, blep);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

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
