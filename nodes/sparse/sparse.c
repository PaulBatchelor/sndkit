#include "graforge.h"
#include "core.h"
#define SK_SPARSE_PRIV
#include "dsp/sparse.h"

struct sparse_n {
    gf_cable *freq;
    gf_cable *out;
    sk_sparse sparse;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct sparse_n *sparse;

    blksize = gf_node_blksize(node);

    sparse = (struct sparse_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, freq;
        freq = gf_cable_get(sparse->freq, n);

        sk_sparse_freq(&sparse->sparse, freq);
        out = sk_sparse_tick(&sparse->sparse);
        gf_cable_set(sparse->out, n, out);
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

int gf_node_sparse(gf_node *node, unsigned long seed)
{
    int rc;
    gf_patch *patch;
    int sr;
    void *ud;
    struct sparse_n *sparse;

    rc = gf_node_get_patch(node, &patch);

    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct sparse_n), &ud);

    if (rc != GF_OK) return rc;

    sparse = (struct sparse_n *)ud;
    sr = gf_patch_srate_get(patch);
    sk_sparse_init(&sparse->sparse, sr, seed);

    rc = gf_node_cables_alloc(node, 2);

    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &sparse->freq);
    gf_node_get_cable(node, 1, &sparse->out);

    gf_node_set_data(node, sparse);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_sparse(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_sparse(node, sk_core_rand(core));

    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}
