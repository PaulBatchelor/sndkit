#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_ADSR_PRIV
#include "dsp/adsr.h"

struct adsr_n {
    gf_cable *gt;
    gf_cable *atk;
    gf_cable *dec;
    gf_cable *sus;
    gf_cable *rel;
    gf_cable *out;
    sk_adsr adsr;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct adsr_n *adsr;

    blksize = gf_node_blksize(node);

    adsr = (struct adsr_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT gt, atk, dec, sus, rel, out;

        gt = gf_cable_get(adsr->gt, n);
        atk = gf_cable_get(adsr->atk, n);
        dec = gf_cable_get(adsr->dec, n);
        rel = gf_cable_get(adsr->rel, n);
        sus = gf_cable_get(adsr->sus, n);

        sk_adsr_attack(&adsr->adsr, atk);
        sk_adsr_decay(&adsr->adsr, dec);
        sk_adsr_sustain(&adsr->adsr, sus);
        sk_adsr_release(&adsr->adsr, rel);

        out = sk_adsr_tick(&adsr->adsr, gt);
        gf_cable_set(adsr->out, n, out);
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

int sk_node_adsr(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param gt, atk, sus, dec, rel;
    void *ud;
    struct adsr_n *adsr;
    int sr;

    rc = sk_param_get(core, &rel);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &sus);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &dec);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &atk);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &gt);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct adsr_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    adsr = (struct adsr_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_adsr_init(&adsr->adsr, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 6);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 5);

    gf_node_get_cable(node, 0, &adsr->gt);
    gf_node_get_cable(node, 1, &adsr->atk);
    gf_node_get_cable(node, 2, &adsr->dec);
    gf_node_get_cable(node, 3, &adsr->sus);
    gf_node_get_cable(node, 4, &adsr->rel);
    gf_node_get_cable(node, 5, &adsr->out);

    gf_node_set_data(node, adsr);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &gt, 0);
    sk_param_set(core, node, &atk, 1);
    sk_param_set(core, node, &dec, 2);
    sk_param_set(core, node, &sus, 3);
    sk_param_set(core, node, &rel, 4);
    sk_param_out(core, node, 5);
    return 0;
}
