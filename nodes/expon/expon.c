#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_EXPON_PRIV
#include "dsp/expon.h"

struct expon_n {
    pw_cable *trig;
    pw_cable *a;
    pw_cable *dur;
    pw_cable *b;
    pw_cable *out;
    sk_expon expon;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct expon_n *expon;

    blksize = pw_node_blksize(node);

    expon = (struct expon_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT trig, a, dur, b, out;
        trig = pw_cable_get(expon->trig, n);
        a = pw_cable_get(expon->a, n);
        dur = pw_cable_get(expon->dur, n);
        b = pw_cable_get(expon->b, n);

        sk_expon_a(&expon->expon, a);
        sk_expon_dur(&expon->expon, dur);
        sk_expon_b(&expon->expon, b);
        out = sk_expon_tick(&expon->expon, trig);
        pw_cable_set(expon->out, n, out);
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

int sk_node_expon(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct expon_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    expon = (struct expon_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_expon_init(&expon->expon, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 5);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 4);

    pw_node_get_cable(node, 0, &expon->trig);
    pw_node_get_cable(node, 1, &expon->a);
    pw_node_get_cable(node, 2, &expon->dur);
    pw_node_get_cable(node, 3, &expon->b);
    pw_node_get_cable(node, 4, &expon->out);

    pw_node_set_data(node, expon);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &a, 1);
    sk_param_set(core, node, &dur, 2);
    sk_param_set(core, node, &b, 3);
    sk_param_out(core, node, 4);
    return 0;
}
