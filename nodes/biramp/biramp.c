#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#include "dsp/biramp.h"

struct biramp_n {
    pw_cable *in;
    pw_cable *pos;
    pw_cable *out;
    SKFLT lphs;
    SKFLT lpos;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct biramp_n *biramp;

    blksize = pw_node_blksize(node);

    biramp = (struct biramp_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in, pos, out;
        in = pw_cable_get(biramp->in, n);
        pos = pw_cable_get(biramp->pos, n);

        out = sk_biramp_stateful(in, pos,
                                 &biramp->lphs,
                                 &biramp->lpos);
        pw_cable_set(biramp->out, n, out);
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

int sk_node_biramp(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in, pos;
    void *ud;
    struct biramp_n *biramp;

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct biramp_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    biramp = (struct biramp_n *)ud;

    biramp->lpos = -1;
    biramp->lphs = -1;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 3);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 2);

    pw_node_get_cable(node, 0, &biramp->in);
    pw_node_get_cable(node, 1, &biramp->pos);
    pw_node_get_cable(node, 2, &biramp->out);

    pw_node_set_data(node, biramp);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &pos, 1);
    sk_param_out(core, node, 2);
    return 0;
}
