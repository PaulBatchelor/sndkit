#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_PHSCLK_PRIV
#include "dsp/phsclk.h"

struct phsclk_n {
    pw_cable *in;
    pw_cable *nticks;
    pw_cable *out;
    sk_phsclk phsclk;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct phsclk_n *phsclk;

    blksize = pw_node_blksize(node);

    phsclk = (struct phsclk_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, nticks, out;
        in = pw_cable_get(phsclk->in, n);

        nticks = pw_cable_get(phsclk->nticks, n);
        sk_phsclk_nticks(&phsclk->phsclk, nticks);
        out = sk_phsclk_tick(&phsclk->phsclk, in);
        pw_cable_set(phsclk->out, n, out);
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

int sk_node_phsclk(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, nticks;
    void *ud;
    struct phsclk_n *phsclk;

    rc = sk_param_get(core, &nticks);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct phsclk_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    phsclk = (struct phsclk_n *)ud;

    sk_phsclk_init(&phsclk->phsclk);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &phsclk->in);
    pw_node_get_cable(node, 1, &phsclk->nticks);
    pw_node_get_cable(node, 2, &phsclk->out);

    pw_node_set_data(node, phsclk);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &nticks, 1);
    sk_param_out(core, node, 2);
    return 0;
}
