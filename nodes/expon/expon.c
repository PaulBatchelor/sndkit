#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_EXPON_PRIV
#include "dsp/expon.h"

struct expon_n {
    gf_cable *trig;
    gf_cable *a;
    gf_cable *dur;
    gf_cable *b;
    gf_cable *out;
    sk_expon expon;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct expon_n *expon;

    blksize = gf_node_blksize(node);

    expon = (struct expon_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, a, dur, b, out;
        trig = gf_cable_get(expon->trig, n);
        a = gf_cable_get(expon->a, n);
        dur = gf_cable_get(expon->dur, n);
        b = gf_cable_get(expon->b, n);

        sk_expon_a(&expon->expon, a);
        sk_expon_dur(&expon->expon, dur);
        sk_expon_b(&expon->expon, b);
        out = sk_expon_tick(&expon->expon, trig);
        gf_cable_set(expon->out, n, out);
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

int sk_node_expon(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig, a, dur, b;
    void *ud;
    struct expon_n *expon;
    int sr;

    rc = sk_param_get(core, &b);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &dur);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &a);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct expon_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    expon = (struct expon_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_expon_init(&expon->expon, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 4);

    gf_node_get_cable(node, 0, &expon->trig);
    gf_node_get_cable(node, 1, &expon->a);
    gf_node_get_cable(node, 2, &expon->dur);
    gf_node_get_cable(node, 3, &expon->b);
    gf_node_get_cable(node, 4, &expon->out);

    gf_node_set_data(node, expon);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &a, 1);
    sk_param_set(core, node, &dur, 2);
    sk_param_set(core, node, &b, 3);
    sk_param_out(core, node, 4);
    return 0;
}
