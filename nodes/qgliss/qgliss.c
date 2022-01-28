#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_QGLISS_PRIV
#include "dsp/qgliss.h"

struct qgliss_n {
    gf_cable *in;
    gf_cable *phs;
    gf_cable *gliss;
    gf_cable *out;
    sk_qgliss qgliss;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct qgliss_n *qgliss;

    blksize = gf_node_blksize(node);

    qgliss = (struct qgliss_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, phs, gliss, out;

        in = gf_cable_get(qgliss->in, n);
        phs = gf_cable_get(qgliss->phs, n);
        gliss = gf_cable_get(qgliss->gliss, n);
        sk_qgliss_gliss(&qgliss->qgliss, gliss);
        out = sk_qgliss_tick(&qgliss->qgliss, in, phs);
        gf_cable_set(qgliss->out, n, out);
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

int sk_node_qgliss(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param phs;
    sk_param gliss;
    sk_table *tab;
    void *ud;
    struct qgliss_n *qgliss;

    rc = sk_param_get(core, &gliss);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &phs);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);
    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);
    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct qgliss_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    qgliss = (struct qgliss_n *)ud;

    sk_qgliss_init(&qgliss->qgliss,
                   sk_table_data(tab),
                   sk_table_size(tab));

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &qgliss->in);
    gf_node_get_cable(node, 1, &qgliss->phs);
    gf_node_get_cable(node, 2, &qgliss->gliss);
    gf_node_get_cable(node, 3, &qgliss->out);

    gf_node_set_data(node, qgliss);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &phs, 1);
    sk_param_set(core, node, &gliss, 2);
    sk_param_out(core, node, 3);
    return 0;
}
