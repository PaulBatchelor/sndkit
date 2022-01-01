#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_DBLIN_PRIV
#include "dsp/dblin.h"

struct dblin_n {
    gf_cable *db;
    gf_cable *out;
    sk_dblin dblin;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct dblin_n *dblin;

    blksize = gf_node_blksize(node);

    dblin = (struct dblin_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT out, db;
        db = gf_cable_get(dblin->db, n);
        out = sk_dblin_tick(&dblin->dblin, db);
        gf_cable_set(dblin->out, n, out);
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

int sk_node_dblin(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param db;
    void *ud;
    struct dblin_n *dblin;

    rc = sk_param_get(core, &db);
    SK_ERROR_CHECK(rc);

    if (sk_param_isconstant(&db)) {
        SKFLT x;
        /* no node needed, just a constant */

        x = sk_param_constant(&db);
        rc = sk_core_constant(core, pow(10, x / 20));

        SK_ERROR_CHECK(rc);
        return 0;
    }

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct dblin_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    dblin = (struct dblin_n *)ud;

    sk_dblin_init(&dblin->dblin);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &dblin->db);
    gf_node_get_cable(node, 1, &dblin->out);

    gf_node_set_data(node, dblin);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &db, 0);
    sk_param_out(core, node, 1);
    return 0;
}
