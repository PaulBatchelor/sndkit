#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"

#define SK_GLOTTIS_PRIV
#include "dsp/glottis.h"

struct glottis_n {
    pw_cable *freq;
    pw_cable *tenseness;
    pw_cable *out;
    sk_glottis glottis;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct glottis_n *glottis;

    blksize = pw_node_blksize(node);

    glottis = (struct glottis_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, freq, tenseness;

        freq = pw_cable_get(glottis->freq, n);
        sk_glottis_freq(&glottis->glottis, freq);

        tenseness = pw_cable_get(glottis->tenseness, n);
        sk_glottis_tenseness(&glottis->glottis, tenseness);

        out = sk_glottis_tick(&glottis->glottis);
        pw_cable_set(glottis->out, n, out);
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

int sk_node_glottis(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq, tenseness;
    void *ud;
    struct glottis_n *glottis;
    int sr;

    rc = sk_param_get(core, &tenseness);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct glottis_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    glottis = (struct glottis_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_glottis_init(&glottis->glottis, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &glottis->freq);
    pw_node_get_cable(node, 1, &glottis->tenseness);
    pw_node_get_cable(node, 2, &glottis->out);

    pw_node_set_data(node, glottis);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &tenseness, 1);
    sk_param_out(core, node, 2);
    return 0;
}
