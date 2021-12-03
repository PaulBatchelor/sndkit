#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_VALP1_PRIV
#include "dsp/valp1.h"

struct valp1_n {
    gf_cable *in;
    gf_cable *freq;
    gf_cable *out;
    sk_valp1 valp1;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct valp1_n *valp1;

    blksize = gf_node_blksize(node);

    valp1 = (struct valp1_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, freq, out;
        in = gf_cable_get(valp1->in, n);
        freq = gf_cable_get(valp1->freq, n);
        sk_valp1_freq(&valp1->valp1, freq);

        out = sk_valp1_tick(&valp1->valp1, in);
        gf_cable_set(valp1->out, n, out);
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

int sk_node_valp1(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, freq;
    void *ud;
    struct valp1_n *valp1;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct valp1_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    valp1 = (struct valp1_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_valp1_init(&valp1->valp1, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &valp1->in);
    gf_node_get_cable(node, 1, &valp1->freq);
    gf_node_get_cable(node, 2, &valp1->out);

    gf_node_set_data(node, valp1);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_out(core, node, 2);
    return 0;
}
