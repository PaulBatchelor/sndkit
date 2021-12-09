#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TSEQ_PRIV
#include "dsp/tseq.h"

struct tseq_n {
    gf_cable *trig;
    gf_cable *mode;
    gf_cable *out;
    sk_tseq tseq;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tseq_n *tseq;

    blksize = gf_node_blksize(node);

    tseq = (struct tseq_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, out;
        trig = gf_cable_get(tseq->trig, n);
        out = sk_tseq_tick(&tseq->tseq, trig);
        gf_cable_set(tseq->out, n, out);
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

int sk_node_tseq(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param mode;
    void *ud;
    struct tseq_n *tseq;
    sk_table *tab;

    rc = sk_param_get(core, &mode);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tseq_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tseq = (struct tseq_n *)ud;

    sk_tseq_init(&tseq->tseq, sk_table_data(tab), sk_table_size(tab));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &tseq->trig);
    gf_node_get_cable(node, 1, &tseq->mode);
    gf_node_get_cable(node, 2, &tseq->out);

    gf_node_set_data(node, tseq);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &mode, 1);
    sk_param_out(core, node, 2);
    return 0;
}
