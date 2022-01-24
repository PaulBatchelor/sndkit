#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"

struct plotter_n {
    gf_cable *in;
    gf_cable *trig;
    FILE *fp;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct plotter_n *plotter;

    blksize = gf_node_blksize(node);

    plotter = (struct plotter_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, trig;
        in = gf_cable_get(plotter->in, n);
        trig = gf_cable_get(plotter->trig, n);

        if (trig != 0) {
            fprintf(plotter->fp, "%g\n", in);
        }
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct plotter_n *plotter;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    plotter = ud;
    fclose(plotter->fp);
    gf_memory_free(patch, &ud);
}

int sk_node_plotter(sk_core *core, const char *filename)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param trig;
    void *ud;
    struct plotter_n *plotter;

    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct plotter_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    plotter = (struct plotter_n *)ud;
    plotter->fp = fopen(filename, "w");

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_get_cable(node, 0, &plotter->in);
    gf_node_get_cable(node, 1, &plotter->trig);

    gf_node_set_data(node, plotter);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &trig, 1);
    return 0;
}
