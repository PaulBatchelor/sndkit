#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_PHASOR_PRIV
#include "dsp/phasor.h"

struct phasor_n {
    gf_cable *freq;
    gf_cable *out;
    sk_phasor phasor;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct phasor_n *phasor;

    blksize = gf_node_blksize(node);

    phasor = (struct phasor_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, f;
        f = gf_cable_get(phasor->freq, n);

        sk_phasor_freq(&phasor->phasor, f);
        out = sk_phasor_tick(&phasor->phasor);

        gf_cable_set(phasor->out, n, out);
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

int sk_node_phasor(sk_core *core, SKFLT iphs)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct phasor_n *phasor;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct phasor_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    phasor = (struct phasor_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_phasor_init(&phasor->phasor, sr, iphs);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &phasor->freq);
    gf_node_get_cable(node, 1, &phasor->out);

    gf_node_set_data(node, phasor);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}

struct tphasor_n {
    gf_cable *reset;
    gf_cable *freq;
    gf_cable *out;
    SKFLT iphs;
    sk_phasor phasor;
};

static void tphasor_compute(gf_node *node)
{
    int blksize;
    int n;
    struct tphasor_n *tp;

    blksize = gf_node_blksize(node);

    tp = (struct tphasor_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT r, f, out;

        r = gf_cable_get(tp->reset, n);
        if (r != 0) {
            sk_phasor_reset(&tp->phasor, tp->iphs);
        }


        f = gf_cable_get(tp->freq, n);
        sk_phasor_freq(&tp->phasor, f);

        out = sk_phasor_tick(&tp->phasor);

        gf_cable_set(tp->out, n, out);
    }
}

int sk_node_tphasor(sk_core *core, SKFLT iphs)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param freq;
    sk_param reset;
    void *ud;
    struct tphasor_n *tphasor;
    int sr;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &reset);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct tphasor_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    tphasor = (struct tphasor_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_phasor_init(&tphasor->phasor, sr, iphs);
    tphasor->iphs = iphs;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &tphasor->reset);
    gf_node_get_cable(node, 1, &tphasor->freq);
    gf_node_get_cable(node, 2, &tphasor->out);

    gf_node_set_data(node, tphasor);
    gf_node_set_compute(node, tphasor_compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &reset, 0);
    sk_param_set(core, node, &freq, 1);
    sk_param_out(core, node, 2);
    return 0;
}
