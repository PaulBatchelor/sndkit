#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#include "dsp/phasewarp.h"

struct phasewarp_n {
    pw_cable *in;
    pw_cable *pos;
    pw_cable *out;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct phasewarp_n *phasewarp;

    blksize = pw_node_blksize(node);

    phasewarp = (struct phasewarp_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, pos, out;
        in = pw_cable_get(phasewarp->in, n);
        pos = pw_cable_get(phasewarp->pos, n);

        out = sk_phasewarp_tick(in, pos);
        pw_cable_set(phasewarp->out, n, out);
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

int sk_node_phasewarp(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, pos;
    void *ud;
    struct phasewarp_n *phasewarp;

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct phasewarp_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    phasewarp = (struct phasewarp_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &phasewarp->in);
    pw_node_get_cable(node, 1, &phasewarp->pos);
    pw_node_get_cable(node, 2, &phasewarp->out);

    pw_node_set_data(node, phasewarp);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &pos, 1);
    sk_param_out(core, node, 2);
    return 0;
}
