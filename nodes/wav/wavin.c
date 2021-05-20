#include "patchwerk.h"
#include "core.h"
#include "wavin.h"

#include "dr_wav.h"

#define BUFSIZE 1024
struct wavin_n {
    pw_cable *out;
    PWFLT buf[BUFSIZE];
    drwav wav;
    int count;
    unsigned long pos;
    unsigned long buf_start;
    unsigned long buf_end;
};

static void read_block(struct wavin_n *p, unsigned long position)
{
    unsigned long samps_read;
    drwav_seek_to_pcm_frame(&p->wav, position);
    samps_read = drwav_read_pcm_frames_f32(&p->wav,
                                           BUFSIZE,
                                           p->buf);
    p->buf_start = position;
    p->buf_end = position + samps_read - 1;
}

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct wavin_n *wavin;

    blksize = pw_node_blksize(node);

    wavin = (struct wavin_n *)pw_node_get_data(node);

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

        pw_cable_set(wavin->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    struct wavin_n *wavin;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);

    wavin = (struct wavin_n *)ud;

    drwav_uninit(&wavin->wav);
    pw_memory_free(patch, &ud);
}


int sk_node_wavin(sk_core *core, const char *filename)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    void *ud;
    struct wavin_n *wavin;

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct wavin_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    wavin = (struct wavin_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 1);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 0);
    pw_node_get_cable(node, 0, &wavin->out);

    /* TODO: check sample rate and channels */
    if (!drwav_init_file(&wavin->wav, filename, NULL)) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        return 1;
    }

    wavin->count = 0;
    wavin->pos = 0;
    wavin->buf_start = 0;
    wavin->buf_end = 0;

    pw_node_set_data(node, wavin);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_out(core, node, 0);
    return 0;
}
