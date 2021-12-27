#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

#define SK_GLOTTIS_PRIV
#include "dsp/glottis.h"

struct glottis_n {
    gf_cable *freq;
    gf_cable *tenseness;
    gf_cable *out;
    sk_glottis glottis;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct glottis_n *glottis;

    blksize = gf_node_blksize(node);

    glottis = (struct glottis_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, freq, tenseness;

        freq = gf_cable_get(glottis->freq, n);
        sk_glottis_freq(&glottis->glottis, freq);

        tenseness = gf_cable_get(glottis->tenseness, n);
        sk_glottis_tenseness(&glottis->glottis, tenseness);

        out = sk_glottis_tick(&glottis->glottis);
        gf_cable_set(glottis->out, n, out);
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

int gf_node_glottis(gf_node *node)
{
    int rc;
    gf_patch *patch;
    void *ud;
    struct glottis_n *glottis;
    int sr;

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct glottis_n), &ud);
    if (rc != GF_OK) return rc;
    glottis = (struct glottis_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_glottis_init(&glottis->glottis, sr);

    rc = gf_node_cables_alloc(node, 3);
    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &glottis->freq);
    gf_node_get_cable(node, 1, &glottis->tenseness);
    gf_node_get_cable(node, 2, &glottis->out);

    gf_node_set_data(node, glottis);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_glottis(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq, tenseness;
    struct glottis_n *g;

    rc = sk_param_get(core, &tenseness);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_glottis(node);
    SK_GF_ERROR_CHECK(rc);

    g = gf_node_get_data(node);

    sk_glottis_srand(&g->glottis, sk_core_rand(core));

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &tenseness, 1);
    sk_param_out(core, node, 2);
    return 0;
}
