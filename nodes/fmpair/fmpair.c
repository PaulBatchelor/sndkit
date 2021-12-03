#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_FMPAIR_PRIV
#include "dsp/fmpair.h"

struct fmpair_n {
    gf_cable *freq;
    gf_cable *car;
    gf_cable *mod;
    gf_cable *index;
    gf_cable *feedback;
    gf_cable *out;
    sk_fmpair_fdbk fmpair;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct fmpair_n *fmpair;

    blksize = gf_node_blksize(node);

    fmpair = (struct fmpair_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out;
        GFFLT car;
        GFFLT mod;
        GFFLT index;
        GFFLT feedback;
        GFFLT freq;
        freq = gf_cable_get(fmpair->freq, n);
        car = gf_cable_get(fmpair->car, n);
        mod = gf_cable_get(fmpair->mod, n);
        index = gf_cable_get(fmpair->index, n);
        feedback = gf_cable_get(fmpair->feedback, n);

        sk_fmpair_freq(&fmpair->fmpair.fmpair, freq);
        sk_fmpair_carrier(&fmpair->fmpair.fmpair, car);
        sk_fmpair_modulator(&fmpair->fmpair.fmpair, mod);
        sk_fmpair_modindex(&fmpair->fmpair.fmpair, index);
        sk_fmpair_fdbk_amt(&fmpair->fmpair, feedback);
        out = sk_fmpair_fdbk_tick(&fmpair->fmpair);

        gf_cable_set(fmpair->out, n, out);
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

int sk_node_fmpair(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    sk_param car;
    sk_param mod;
    sk_param index;
    sk_param feedback;
    void *ud;
    struct fmpair_n *fmpair;
    int sr;
    sk_table *tab;
    SKFLT *data;
    int sz;

    rc = sk_param_get(core, &feedback);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &index);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &mod);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &car);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct fmpair_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    fmpair = (struct fmpair_n *)ud;

    sr = gf_patch_srate_get(patch);

    data = sk_table_data(tab);
    sz = sk_table_size(tab);
    sk_fmpair_fdbk_init(&fmpair->fmpair, sr,
                        data, sz, 0,
                        data, sz, 0);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 6);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 5);

    gf_node_get_cable(node, 0, &fmpair->freq);
    gf_node_get_cable(node, 1, &fmpair->car);
    gf_node_get_cable(node, 2, &fmpair->mod);
    gf_node_get_cable(node, 3, &fmpair->index);
    gf_node_get_cable(node, 4, &fmpair->feedback);
    gf_node_get_cable(node, 5, &fmpair->out);

    gf_node_set_data(node, fmpair);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &car, 1);
    sk_param_set(core, node, &mod, 2);
    sk_param_set(core, node, &index, 3);
    sk_param_set(core, node, &feedback, 4);
    sk_param_out(core, node, 5);
    return 0;
}

/* TODO: refactor better. DRY */

int sk_node_fmpair2(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    sk_param car;
    sk_param mod;
    sk_param index;
    sk_param feedback;
    void *ud;
    struct fmpair_n *fmpair;
    int sr;
    sk_table *tab[2];
    SKFLT *data[2];
    int sz[2];

    rc = sk_param_get(core, &feedback);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &index);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &mod);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &car);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab[1]);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab[0]);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct fmpair_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    fmpair = (struct fmpair_n *)ud;

    sr = gf_patch_srate_get(patch);

    data[0] = sk_table_data(tab[0]);
    sz[0] = sk_table_size(tab[0]);

    data[1] = sk_table_data(tab[1]);
    sz[1] = sk_table_size(tab[1]);
    sk_fmpair_fdbk_init(&fmpair->fmpair, sr,
                        data[0], sz[0], 0,
                        data[1], sz[1], 0);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 6);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 5);

    gf_node_get_cable(node, 0, &fmpair->freq);
    gf_node_get_cable(node, 1, &fmpair->car);
    gf_node_get_cable(node, 2, &fmpair->mod);
    gf_node_get_cable(node, 3, &fmpair->index);
    gf_node_get_cable(node, 4, &fmpair->feedback);
    gf_node_get_cable(node, 5, &fmpair->out);

    gf_node_set_data(node, fmpair);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &car, 1);
    sk_param_set(core, node, &mod, 2);
    sk_param_set(core, node, &index, 3);
    sk_param_set(core, node, &feedback, 4);
    sk_param_out(core, node, 5);
    return 0;
}
