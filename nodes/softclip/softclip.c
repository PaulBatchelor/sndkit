#include "graforge.h"
#include "core.h"
#include "dsp/softclip.h"

struct softclip_n {
    gf_cable *in;
    gf_cable *drive;
    gf_cable *out;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct softclip_n *softclip;

    blksize = gf_node_blksize(node);

    softclip = (struct softclip_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, drive, out;
        drive = gf_cable_get(softclip->drive, n);
        in = gf_cable_get(softclip->in, n);

        out = sk_softclip_tick(in, drive);
        gf_cable_set(softclip->out, n, out);
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

int gf_node_softclip(gf_node *node)
{
    int rc;
    gf_patch *patch;
    void *ud;
    struct softclip_n *softclip;

    rc = gf_node_get_patch(node, &patch);

    if (rc != GF_OK) return rc;


    rc = gf_memory_alloc(patch, sizeof(struct softclip_n), &ud);

    if (rc != GF_OK) return rc;

    softclip = (struct softclip_n *)ud;
    rc = gf_node_cables_alloc(node, 3);
    if (rc != GF_OK) return rc;

    gf_node_set_block(node, 2);

    gf_node_get_cable(node, 0, &softclip->in);
    gf_node_get_cable(node, 1, &softclip->drive);
    gf_node_get_cable(node, 2, &softclip->out);

    gf_node_set_data(node, softclip);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_softclip(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param drive;

    rc = sk_param_get(core, &drive);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_softclip(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &drive, 1);
    sk_param_out(core, node, 2);
    return 0;
}
