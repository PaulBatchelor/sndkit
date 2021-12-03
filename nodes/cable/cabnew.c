/*
 * cabnew
 *
 * This node creates an internal cable with a buffer. It will
 * copy an input signal to this cable, which is then treated
 * as an output cable. Use this if a patch is stressing out
 * the buffer pool.
 */

#include "graforge.h"
#include "core.h"

typedef struct {
    gf_cable *in;
    gf_cable *out;
    gf_buffer *buf;
} cabnew_d;

static void compute(gf_node *node)
{
    int n, blksize;
    cabnew_d *c;
    GFFLT in;

    c = gf_node_get_data(node);
    blksize = gf_node_blksize(node);

    for (n = 0; n < blksize; n++) {
        in = gf_cable_get(c->in, n);
        gf_cable_set(c->out, n, in);
    }
}


static void destroy(gf_node *node)
{
    cabnew_d *c;
    int rc;
    gf_patch *patch;

    gf_node_cables_free(node);
    c = gf_node_get_data(node);

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;

    gf_buffer_free(patch, c->buf);

    rc = gf_memory_free(patch, (void **)&c->buf);
    if (rc != GF_OK) return;

    rc = gf_memory_free(patch, (void **)&c);
    if (rc != GF_OK) return;
}

int gf_node_cabnew(gf_node *node)
{
    int blksize;
    cabnew_d *c;
    gf_patch *patch;
    int rc;
    GFFLT *blk;
    gf_stack *stack;

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return rc;

    blksize = gf_node_blksize(node);


    rc = gf_memory_alloc(patch,
                         sizeof(cabnew_d),
                         (void **)&c);

    if (rc != GF_OK) return rc;


    gf_node_cables_alloc(node, 2);
    gf_node_get_cable(node, 0, &c->in);
    gf_node_get_cable(node, 1, &c->out);

    /* create new buffer */
    rc = gf_memory_alloc(patch,
                         gf_buffer_size(),
                         (void **)&c->buf);
    if (rc != GF_OK) return rc;

    gf_buffer_alloc(patch, c->buf, blksize);
    gf_buffer_init(c->buf);

    /* hold the buffer */
    gf_buffer_holdu(c->buf);

    /* bind this new buffer to the output cable */
    blk = gf_buffer_data(c->buf);
    gf_cable_set_block(c->out, blk, blksize);
    gf_cable_set_buffer(c->out, c->buf);

    /* push buffer to buffer stack */
    stack = gf_patch_stack(patch);

    gf_stack_push_buffer(stack, c->buf);

    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);
    gf_node_set_data(node, c);

    return GF_OK;
}

int sk_node_cabnew(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;

    rc = sk_param_get_cable(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);
    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cabnew(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);
    return 0;
}
