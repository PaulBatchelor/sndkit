#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_EUCLID_PRIV
#include "dsp/euclid.h"

struct euclid_n {
    gf_cable *trig;
    gf_cable *pulses;
    gf_cable *length;
    gf_cable *out;
    sk_euclid euclid;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct euclid_n *euclid;

    blksize = gf_node_blksize(node);

    euclid = (struct euclid_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, pulses, length, out;

        trig = gf_cable_get(euclid->trig, n);

        pulses = gf_cable_get(euclid->pulses, n);
        sk_euclid_pulses(&euclid->euclid, pulses);

        length = gf_cable_get(euclid->length, n);
        sk_euclid_length(&euclid->euclid, length);

        out = sk_euclid_tick(&euclid->euclid, trig);
        gf_cable_set(euclid->out, n, out);
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

int sk_node_euclid(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param pulses;
    sk_param length;
    void *ud;
    struct euclid_n *euclid;

    rc = sk_param_get(core, &length);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &pulses);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);
    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct euclid_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    euclid = (struct euclid_n *)ud;

    sk_euclid_init(&euclid->euclid);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &euclid->trig);
    gf_node_get_cable(node, 1, &euclid->pulses);
    gf_node_get_cable(node, 2, &euclid->length);
    gf_node_get_cable(node, 3, &euclid->out);

    gf_node_set_data(node, euclid);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &pulses, 1);
    sk_param_set(core, node, &length, 2);
    sk_param_out(core, node, 3);
    return 0;
}
