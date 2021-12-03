
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

struct arith_n {
    gf_cable *a;
    gf_cable *b;
    gf_cable *out;
};

static void add_compute(gf_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = gf_node_blksize(node);

    arith = (struct arith_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT a, b, out;

        a = gf_cable_get(arith->a, n);
        b = gf_cable_get(arith->b, n);
        out = a + b;

        gf_cable_set(arith->out, n, out);
    }
}

static void mul_compute(gf_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = gf_node_blksize(node);

    arith = (struct arith_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT a, b, out;

        a = gf_cable_get(arith->a, n);
        b = gf_cable_get(arith->b, n);
        out = a * b;

        gf_cable_set(arith->out, n, out);
    }
}

static void div_compute(gf_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = gf_node_blksize(node);

    arith = (struct arith_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT a, b, out;

        a = gf_cable_get(arith->a, n);
        b = gf_cable_get(arith->b, n);
        out = a / b; /* watch out for divide by 0 */

        gf_cable_set(arith->out, n, out);
    }
}

static void sub_compute(gf_node *node)
{
    int blksize;
    int n;
    struct arith_n *arith;

    blksize = gf_node_blksize(node);

    arith = (struct arith_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT a, b, out;

        a = gf_cable_get(arith->a, n);
        b = gf_cable_get(arith->b, n);
        out = a - b;

        gf_cable_set(arith->out, n, out);
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

static int node_arith(sk_core *core, gf_function compute)
{
    gf_patch *patch;
    gf_node *node;
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

    rc = gf_memory_alloc(patch, sizeof(struct arith_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    arith = (struct arith_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 3);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &arith->a);
    gf_node_get_cable(node, 1, &arith->b);
    gf_node_get_cable(node, 2, &arith->out);

    gf_node_set_data(node, arith);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

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

int sk_node_sub(sk_core *core)
{
    return node_arith(core, sub_compute);
}

int sk_node_div(sk_core *core)
{
    return node_arith(core, div_compute);
}
