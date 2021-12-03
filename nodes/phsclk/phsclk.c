#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_PHSCLK_PRIV
#include "dsp/phsclk.h"

struct phsclk_n {
    gf_cable *in;
    gf_cable *nticks;
    gf_cable *out;
    sk_phsclk phsclk;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct phsclk_n *phsclk;

    blksize = gf_node_blksize(node);

    phsclk = (struct phsclk_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, nticks, out;
        in = gf_cable_get(phsclk->in, n);

        nticks = gf_cable_get(phsclk->nticks, n);
        sk_phsclk_nticks(&phsclk->phsclk, nticks);
        out = sk_phsclk_tick(&phsclk->phsclk, in);
        gf_cable_set(phsclk->out, n, out);
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

int sk_node_phsclk(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in, nticks;
    void *ud;
    struct phsclk_n *phsclk;

    rc = sk_param_get(core, &nticks);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct phsclk_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    phsclk = (struct phsclk_n *)ud;

    sk_phsclk_init(&phsclk->phsclk);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &phsclk->in);
    gf_node_get_cable(node, 1, &phsclk->nticks);
    gf_node_get_cable(node, 2, &phsclk->out);

    gf_node_set_data(node, phsclk);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &nticks, 1);
    sk_param_out(core, node, 2);
    return 0;
}
