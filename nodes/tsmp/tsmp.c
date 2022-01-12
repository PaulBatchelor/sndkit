#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TSMP_PRIV
#include "dsp/tsmp.h"

struct tsmp_n {
    gf_cable *trig;
    gf_cable *rate;
    gf_cable *out;
    sk_tsmp tsmp;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tsmp_n *tsmp;

    blksize = gf_node_blksize(node);

    tsmp = (struct tsmp_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, rate, out;

        trig = gf_cable_get(tsmp->trig, n);
        rate = gf_cable_get(tsmp->rate, n);

        sk_tsmp_rate(&tsmp->tsmp, rate);

        out = sk_tsmp_tick(&tsmp->tsmp, trig);
        gf_cable_set(tsmp->out, n, out);
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

int sk_node_tsmp(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param rate;
    void *ud;
    struct tsmp_n *tsmp;
    sk_table *tab;

    rc = sk_param_get(core, &rate);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tsmp_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tsmp = (struct tsmp_n *)ud;

    sk_tsmp_init(&tsmp->tsmp,
                 sk_table_data(tab),
                 sk_table_size(tab));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &tsmp->trig);
    gf_node_get_cable(node, 1, &tsmp->rate);
    gf_node_get_cable(node, 2, &tsmp->out);

    gf_node_set_data(node, tsmp);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &rate, 1);
    sk_param_out(core, node, 2);
    return 0;
}
