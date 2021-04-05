#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_VALP1_PRIV
#include "dsp/valp1.h"

struct valp1_n {
    pw_cable *in;
    pw_cable *freq;
    pw_cable *out;
    sk_valp1 valp1;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct valp1_n *valp1;

    blksize = pw_node_blksize(node);

    valp1 = (struct valp1_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, freq, out;
        in = pw_cable_get(valp1->in, n);
        freq = pw_cable_get(valp1->freq, n);
        sk_valp1_freq(&valp1->valp1, freq);

        out = sk_valp1_tick(&valp1->valp1, in);
        pw_cable_set(valp1->out, n, out);
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

int sk_node_valp1(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct valp1_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    valp1 = (struct valp1_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_valp1_init(&valp1->valp1, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &valp1->in);
    pw_node_get_cable(node, 1, &valp1->freq);
    pw_node_get_cable(node, 2, &valp1->out);

    pw_node_set_data(node, valp1);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_out(core, node, 2);
    return 0;
}
