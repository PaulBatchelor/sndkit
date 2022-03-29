/*
 * Brown
 *
 * Brownian noise algorithm based on implementation found here:
 * http://vellocet.com/dsp/noise/VRand.h
 *
 *
 */

#include <stdlib.h>
#include "graforge.h"
#include "core.h"

#define SK_BROWN_PRIV
#include "brown.h"
#define LCGMAX 2147483648

void sk_brown_init(sk_brown *b, unsigned long seed)
{
    b->brown = 0.0;
    b->rng = seed;
}

static unsigned long lcg(unsigned long rng)
{
    return (1103515245 * rng + 12345) % 2147483648;
}

SKFLT sk_brown_tick(sk_brown *b)
{
    SKFLT out;
    while (1) {
        SKFLT r;
        r = b->rng / (SKFLT)(LCGMAX);
        r = ((r * 2) - 1) * 0.5;
        b->brown += r;
        b->rng = lcg(b->rng);
        if (b->brown < -8.0f || b->brown > 8.0f) {
            b->brown -= r;
        } else {
            break;
        }
    }

    out = b->brown * 0.0625;
    return out;
}

struct brown_n {
    gf_cable *out;
    sk_brown tb;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct brown_n *tb;

    blksize = gf_node_blksize(node);

    tb = (struct brown_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out;

        out = sk_brown_tick(&tb->tb);

        gf_cable_set(tb->out, n, out);
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

int gf_node_brown(gf_node *node, unsigned long seed)
{
    int rc;
    gf_patch *patch;
    struct brown_n *tb;
    void *ud;

    rc = gf_node_get_patch(node, &patch);

    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct brown_n), &ud);
    if (rc != GF_OK) return rc;

    tb = (struct brown_n *)ud;

    sk_brown_init(&tb->tb, seed);

    rc = gf_node_cables_alloc(node, 1);

    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 0);
    gf_node_get_cable(node, 0, &tb->out);


    gf_node_set_data(node, tb);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_brown(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_brown(node, sk_core_rand(core));
    SK_GF_ERROR_CHECK(rc);

    sk_param_out(core, node, 0);
    return 0;
}
