#include "graforge.h"
#include "core.h"

#include "dr_wav.h"

#define BUFSIZE 1024
struct wavin_n {
    gf_cable *out;
    GFFLT buf[BUFSIZE];
    sk_drwav wav;
    int count;
    unsigned long pos;
    unsigned long buf_start;
    unsigned long buf_end;
};

static void read_block(struct wavin_n *p, unsigned long position)
{
    unsigned long samps_read;
    sk_drwav_seek_to_pcm_frame(&p->wav, position);
    samps_read = sk_drwav_read_pcm_frames_f32(&p->wav,
                                           BUFSIZE,
                                           p->buf);
    p->buf_start = position;
    p->buf_end = position + samps_read - 1;
}

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct wavin_n *wavin;

    blksize = gf_node_blksize(node);

    wavin = (struct wavin_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        SKFLT out;

        out = 0;

        if (wavin->pos > wavin->wav.totalPCMFrameCount) {
            out = 0;
        } else {
            if (wavin->count == 0) {
                read_block(wavin, wavin->pos);
            }
            out = wavin->buf[wavin->count];
            /* TODO: dont use modulus, it's expensive */
            wavin->count = (wavin->count + 1) % BUFSIZE;
            wavin->pos++;
        }

        gf_cable_set(wavin->out, n, out);
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct wavin_n *wavin;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);

    wavin = (struct wavin_n *)ud;

    sk_drwav_uninit(&wavin->wav);
    gf_memory_free(patch, &ud);
}


int sk_node_wavin(sk_core *core, const char *filename)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    void *ud;
    struct wavin_n *wavin;

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct wavin_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    wavin = (struct wavin_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 1);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 0);
    gf_node_get_cable(node, 0, &wavin->out);

    /* TODO: check sample rate and channels */
    if (!sk_drwav_init_file(&wavin->wav, filename, NULL)) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        return 1;
    }

    wavin->count = 0;
    wavin->pos = 0;
    wavin->buf_start = 0;
    wavin->buf_end = 0;

    gf_node_set_data(node, wavin);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_out(core, node, 0);
    return 0;
}
