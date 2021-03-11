
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"

struct arith_n {
    pw_cable *a;
    pw_cable *b;
    pw_cable *out;
};

static void add_compute(pw_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = pw_node_blksize(node);

    arith = (struct arith_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT a, b, out;

        a = pw_cable_get(arith->a, n);
        b = pw_cable_get(arith->b, n);
        out = a + b;

        pw_cable_set(arith->out, n, out);
    }
}

static void mul_compute(pw_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = pw_node_blksize(node);

    arith = (struct arith_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT a, b, out;

        a = pw_cable_get(arith->a, n);
        b = pw_cable_get(arith->b, n);
        out = a * b;

        pw_cable_set(arith->out, n, out);
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

static int node_arith(sk_core *core, pw_function compute)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param a;
    sk_param b;
    void *ud;
    struct arith_n *arith;

    rc = sk_param_get(core, &b);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &a);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct arith_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    arith = (struct arith_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &arith->a);
    pw_node_get_cable(node, 1, &arith->b);
    pw_node_get_cable(node, 2, &arith->out);

    pw_node_set_data(node, arith);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &a, 0);
    sk_param_set(core, node, &b, 1);
    sk_param_out(core, node, 2);
    return 0;
}

int sk_node_add(sk_core *core)
{
    return node_arith(core, add_compute);
}

int sk_node_mul(sk_core *core)
{
    return node_arith(core, mul_compute);
}

/* TODO: mul, sub, div */
