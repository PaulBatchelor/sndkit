#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_FMPAIR_PRIV
#include "dsp/fmpair.h"

struct fmpair_n {
    pw_cable *freq;
    pw_cable *car;
    pw_cable *mod;
    pw_cable *index;
    pw_cable *feedback;
    pw_cable *out;
    sk_fmpair_fdbk fmpair;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct fmpair_n *fmpair;

    blksize = pw_node_blksize(node);

    fmpair = (struct fmpair_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out;
        PWFLT car;
        PWFLT mod;
        PWFLT index;
        PWFLT feedback;
        PWFLT freq;
        freq = pw_cable_get(fmpair->freq, n);
        car = pw_cable_get(fmpair->car, n);
        mod = pw_cable_get(fmpair->mod, n);
        index = pw_cable_get(fmpair->index, n);
        feedback = pw_cable_get(fmpair->feedback, n);

        sk_fmpair_freq(&fmpair->fmpair.fmpair, freq);
        sk_fmpair_carrier(&fmpair->fmpair.fmpair, car);
        sk_fmpair_modulator(&fmpair->fmpair.fmpair, mod);
        sk_fmpair_modindex(&fmpair->fmpair.fmpair, index);
        sk_fmpair_fdbk_amt(&fmpair->fmpair, feedback);
        out = sk_fmpair_fdbk_tick(&fmpair->fmpair);

        pw_cable_set(fmpair->out, n, out);
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

int sk_node_fmpair(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct fmpair_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    fmpair = (struct fmpair_n *)ud;

    sr = pw_patch_srate_get(patch);

    data = sk_table_data(tab);
    sz = sk_table_size(tab);
    sk_fmpair_fdbk_init(&fmpair->fmpair, sr,
                        data, sz, 0,
                        data, sz, 0);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 6);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 5);

    pw_node_get_cable(node, 0, &fmpair->freq);
    pw_node_get_cable(node, 1, &fmpair->car);
    pw_node_get_cable(node, 2, &fmpair->mod);
    pw_node_get_cable(node, 3, &fmpair->index);
    pw_node_get_cable(node, 4, &fmpair->feedback);
    pw_node_get_cable(node, 5, &fmpair->out);

    pw_node_set_data(node, fmpair);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_set(core, node, &mod, 1);
    sk_param_set(core, node, &car, 2);
    sk_param_set(core, node, &index, 3);
    sk_param_set(core, node, &feedback, 4);
    sk_param_out(core, node, 5);
    return 0;
}
