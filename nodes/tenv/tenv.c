#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_TENV_PRIV
#include "dsp/tenv.h"

struct tenv_n {
    gf_cable *trig;
    gf_cable *atk;
    gf_cable *hold;
    gf_cable *rel;
    gf_cable *out;
    sk_tenv tenv;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct tenv_n *tenv;

    blksize = gf_node_blksize(node);

    tenv = (struct tenv_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, atk, hold, rel, out;
        trig = gf_cable_get(tenv->trig, n);
        atk = gf_cable_get(tenv->atk, n);
        hold = gf_cable_get(tenv->hold, n);
        rel = gf_cable_get(tenv->rel , n);

        sk_tenv_attack(&tenv->tenv, atk);
        sk_tenv_hold(&tenv->tenv, hold);
        sk_tenv_release(&tenv->tenv, rel);
        out = sk_tenv_tick(&tenv->tenv, trig);
        gf_cable_set(tenv->out, n, out);
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

int sk_node_tenv(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param atk;
    sk_param hold;
    sk_param rel;
    void *ud;
    struct tenv_n *tenv;
    int sr;

    rc = sk_param_get(core, &rel);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &hold);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &atk);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tenv_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tenv = (struct tenv_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_tenv_init(&tenv->tenv, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &tenv->trig);
    gf_node_get_cable(node, 1, &tenv->atk);
    gf_node_get_cable(node, 2, &tenv->hold);
    gf_node_get_cable(node, 3, &tenv->rel);
    gf_node_get_cable(node, 4, &tenv->out);

    gf_node_set_data(node, tenv);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &atk, 1);
    sk_param_set(core, node, &hold, 2);
    sk_param_set(core, node, &rel, 3);
    sk_param_out(core, node, 4);
    return 0;
}
