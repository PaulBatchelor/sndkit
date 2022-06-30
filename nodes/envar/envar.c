#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "graforge.h"
#include "core.h"
#define SK_ENVAR_PRIV
#include "dsp/envar.h"

struct envar_n {
    gf_cable *gate;
    gf_cable *atk;
    gf_cable *rel;
    gf_cable *out;
    sk_envar envar;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct envar_n *envar;

    blksize = gf_node_blksize(node);

    envar = (struct envar_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT gate, atk, rel, out;

        gate = gf_cable_get(envar->gate, n);

        atk = gf_cable_get(envar->atk, n);
        sk_envar_attack(&envar->envar, atk);

        rel = gf_cable_get(envar->rel, n);
        sk_envar_release(&envar->envar, rel);

        out = sk_envar_tick(&envar->envar, gate);
        gf_cable_set(envar->out, n, out);
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

int sk_node_envar(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param gate;
    sk_param atk;
    sk_param rel;
    int sr;
    void *ud;
    struct envar_n *envar;

    rc = sk_param_get(core, &rel);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &atk);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &gate);
    SK_ERROR_CHECK(rc);
    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct envar_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    envar = (struct envar_n *)ud;
    sr = gf_patch_srate_get(patch);

    sk_envar_init(&envar->envar, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &envar->gate);
    gf_node_get_cable(node, 1, &envar->atk);
    gf_node_get_cable(node, 2, &envar->rel);
    gf_node_get_cable(node, 3, &envar->out);

    gf_node_set_data(node, envar);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &gate, 0);
    sk_param_set(core, node, &atk, 1);
    sk_param_set(core, node, &rel, 2);
    sk_param_out(core, node, 3);
    return 0;
}
