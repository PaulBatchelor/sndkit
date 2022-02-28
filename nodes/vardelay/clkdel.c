#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_VARDELAY_PRIV
#include "dsp/vardelay.h"

struct clkdel_n {
    gf_cable *in;
    gf_cable *feedback;
    gf_cable *phs;
    gf_cable *out;
    sk_clkdel clkdel;
    SKFLT *buf;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct clkdel_n *clkdel;

    blksize = gf_node_blksize(node);

    clkdel = (struct clkdel_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, phs, feedback, out;
        in = gf_cable_get(clkdel->in, n);
        feedback = gf_cable_get(clkdel->feedback, n);
        sk_vardelay_feedback(&clkdel->clkdel.vd, feedback);
        phs = gf_cable_get(clkdel->phs, n);

        out = sk_clkdel_tick(&clkdel->clkdel, in, phs);
        gf_cable_set(clkdel->out, n, out);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct clkdel_n *clkdel;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    clkdel = ud;
    ud = clkdel->buf;
    gf_memory_free(patch, &ud);
    ud = clkdel;
    gf_memory_free(patch, &ud);
}

int sk_node_clkdel(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, feedback, phs;
    SKFLT maxdelay;
    void *ud;
    struct clkdel_n *clkdel;
    int sr;
    unsigned int sz;
    unsigned int i;

    rc = sk_param_get_constant(core, &maxdelay);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &phs);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &feedback);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct clkdel_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    clkdel = (struct clkdel_n*)ud;

    sr = gf_patch_srate_get(patch);

    sz = floor(sr * maxdelay);
    rc = gf_memory_alloc(patch, sz * sizeof(SKFLT), &ud);
    SK_GF_ERROR_CHECK(rc);
    clkdel->buf = ud;

    for (i = 0; i < sz; i++) clkdel->buf[i] = 0;

    sk_clkdel_init(&clkdel->clkdel, sr, clkdel->buf, sz);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &clkdel->in);
    gf_node_get_cable(node, 1, &clkdel->feedback);
    gf_node_get_cable(node, 2, &clkdel->phs);
    gf_node_get_cable(node, 3, &clkdel->out);

    gf_node_set_data(node, clkdel);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &feedback, 1);
    sk_param_set(core, node, &phs, 2);
    sk_param_out(core, node, 3);
    return 0;
}
