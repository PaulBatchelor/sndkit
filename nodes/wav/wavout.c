#include "patchwerk.h"
#include "core.h"
#include "wavout.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define BUFSIZE 1024
struct wavout_n {
    pw_cable *in;
    PWFLT buf[BUFSIZE];
    drwav wav;
    int count;
};

static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct wavout_n *wavout;

    blksize = pw_node_blksize(node);

    wavout = (struct wavout_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT in;
        in = pw_cable_get(wavout->in, n);
        wavout->buf[wavout->count] = in;
        wavout->count++;

        if (wavout->count >= BUFSIZE) {
            drwav_write_pcm_frames(&wavout->wav, BUFSIZE, wavout->buf);
            wavout->count = 0;
        }
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    struct wavout_n *wavout;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    wavout = (struct wavout_n *)ud;

    if (wavout->count != 0) {
        drwav_write_pcm_frames(&wavout->wav, wavout->count, wavout->buf);
    }

    drwav_uninit(&wavout->wav);
    pw_memory_free(patch, &ud);
}


int sk_node_wavout(sk_core *core, const char *filename)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct wavout_n *wavout;
    drwav_data_format format;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct wavout_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    wavout = (struct wavout_n *)ud;

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 1);
    SK_PW_ERROR_CHECK(rc);

    pw_node_get_cable(node, 0, &wavout->in);


    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = 1;
    format.sampleRate = pw_patch_srate_get(patch);
    format.bitsPerSample = 32;

    drwav_init_file_write(&wavout->wav, filename, &format, NULL);
    wavout->count = 0;

    pw_node_set_data(node, wavout);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    return 0;
}
