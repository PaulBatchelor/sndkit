#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_REPHASOR_PRIV
#include "dsp/rephasor.h"

struct rephasor_n {
    pw_cable *ext;
    pw_cable *scale;
    pw_cable *out;
    sk_rephasor rephasor;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct rephasor_n *rephasor;

    blksize = pw_node_blksize(node);

    rephasor = (struct rephasor_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, ext, scale;
        ext = pw_cable_get(rephasor->ext, n);
        scale = pw_cable_get(rephasor->scale, n);

        sk_rephasor_scale(&rephasor->rephasor, scale);
        out = sk_rephasor_tick(&rephasor->rephasor, ext);

        pw_cable_set(rephasor->out, n, out);
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

int sk_node_rephasor(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param ext;
    sk_param scale;
    void *ud;
    struct rephasor_n *rephasor;

    rc = sk_param_get(core, &scale);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &ext);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct rephasor_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    rephasor = (struct rephasor_n *)ud;

    sk_rephasor_init(&rephasor->rephasor);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &rephasor->ext);
    pw_node_get_cable(node, 1, &rephasor->scale);
    pw_node_get_cable(node, 2, &rephasor->out);

    pw_node_set_data(node, rephasor);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &ext, 0);
    sk_param_set(core, node, &scale, 1);
    sk_param_out(core, node, 2);
    return 0;
}

struct rephasorx_n {
    pw_cable *ext;
    pw_cable *factor;
    pw_cable *out;
    sk_rephasorx rpx;
};

static void rephasorx_compute(pw_node *node,
                              PWFLT (*tick)(sk_rephasorx *, SKFLT))

{
    int blksize;
    int n;
    struct rephasorx_n *rephasorx;

    blksize = pw_node_blksize(node);

    rephasorx = (struct rephasorx_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, ext, factor;
        ext = pw_cable_get(rephasorx->ext, n);
        factor = pw_cable_get(rephasorx->factor, n);

        sk_rephasorx_factor(&rephasorx->rpx, factor);
        out = tick(&rephasorx->rpx, ext);

        pw_cable_set(rephasorx->out, n, out);
    }
}

static void phsdiv_compute(pw_node *node)
{
    rephasorx_compute(node, sk_rephasorx_tick_div);
}

static void phsmul_compute(pw_node *node)
{
    rephasorx_compute(node, sk_rephasorx_tick_mul);
}

static int node_rephasorx(sk_core *core, pw_function comp)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param ext;
    sk_param factor;
    void *ud;
    struct rephasorx_n *rephasorx;

    rc = sk_param_get(core, &factor);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &ext);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct rephasorx_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    rephasorx = (struct rephasorx_n *)ud;

    sk_rephasorx_init(&rephasorx->rpx);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &rephasorx->ext);
    pw_node_get_cable(node, 1, &rephasorx->factor);
    pw_node_get_cable(node, 2, &rephasorx->out);

    pw_node_set_data(node, rephasorx);
    pw_node_set_compute(node, comp);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &ext, 0);
    sk_param_set(core, node, &factor, 1);
    sk_param_out(core, node, 2);
    return 0;
}

int sk_node_phsdiv(sk_core *core)
{
    return node_rephasorx(core, phsdiv_compute);
}

int sk_node_phsmul(sk_core *core)
{
    return node_rephasorx(core, phsmul_compute);
}
