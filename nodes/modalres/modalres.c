#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_MODALRES_PRIV
#include "dsp/modalres.h"

struct modalres_n {
    pw_cable *in;
    pw_cable *freq;
    pw_cable *q;
    pw_cable *out;
    sk_modalres modalres;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct modalres_n *modalres;

    blksize = pw_node_blksize(node);

    modalres = (struct modalres_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, freq, q, out;
        in = pw_cable_get(modalres->in, n);

        freq = pw_cable_get(modalres->freq, n);
        sk_modalres_freq(&modalres->modalres, freq);
        q = pw_cable_get(modalres->q, n);
        sk_modalres_q(&modalres->modalres, q);

        out = sk_modalres_tick(&modalres->modalres, in);
        pw_cable_set(modalres->out, n, out);
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

int sk_node_modalres(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, freq, q;
    void *ud;
    struct modalres_n *modalres;
    int sr;

    rc = sk_param_get(core, &q);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct modalres_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    modalres = (struct modalres_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_modalres_init(&modalres->modalres, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 4);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 3);

    pw_node_get_cable(node, 0, &modalres->in);
    pw_node_get_cable(node, 1, &modalres->freq);
    pw_node_get_cable(node, 2, &modalres->q);
    pw_node_get_cable(node, 3, &modalres->out);

    pw_node_set_data(node, modalres);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_set(core, node, &q, 2);
    sk_param_out(core, node, 3);
    return 0;
}
