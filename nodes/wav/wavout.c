#include "graforge.h"
#include "core.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define BUFSIZE 1024
#define BUFSIZE_DIV2 512

struct wavout_n {
    gf_cable *inL;
    gf_cable *inR;
    GFFLT buf[BUFSIZE];
    sk_drwav wav;
    int count;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct wavout_n *wavout;

    blksize = gf_node_blksize(node);

    wavout = (struct wavout_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in;
        in = gf_cable_get(wavout->inL, n);
        wavout->buf[wavout->count] = in;
        wavout->count++;

        if (wavout->count >= BUFSIZE) {
            sk_drwav_write_pcm_frames(&wavout->wav, BUFSIZE, wavout->buf);
            wavout->count = 0;
        }
    }
}

static void s_compute(gf_node *node)
{
    int blksize;
    int n;
    struct wavout_n *wavout;

    blksize = gf_node_blksize(node);

    wavout = (struct wavout_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT inL, inR;
        inL = gf_cable_get(wavout->inL, n);
        inR = gf_cable_get(wavout->inR, n);
        wavout->buf[2*wavout->count] = inL;
        wavout->buf[2*wavout->count + 1] = inR;
        wavout->count++;

        if (wavout->count >= BUFSIZE_DIV2) {
            sk_drwav_write_pcm_frames(&wavout->wav,
                                      BUFSIZE_DIV2,
                                      wavout->buf);
            wavout->count = 0;
        }
    }
}

static void destroy(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct wavout_n *wavout;
    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return;
    gf_node_cables_free(node);
    ud = gf_node_get_data(node);
    wavout = (struct wavout_n *)ud;

    if (wavout->count != 0) {
        sk_drwav_write_pcm_frames(&wavout->wav, wavout->count, wavout->buf);
    }

    sk_drwav_uninit(&wavout->wav);
    gf_memory_free(patch, &ud);
}


int sk_node_wavout(sk_core *core, const char *filename)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    void *ud;
    struct wavout_n *wavout;
    sk_drwav_data_format format;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct wavout_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    wavout = (struct wavout_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 1);
    SK_GF_ERROR_CHECK(rc);

    gf_node_get_cable(node, 0, &wavout->inL);
    wavout->inR = NULL; /* unused */


    format.container = sk_drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = 1;
    format.sampleRate = gf_patch_srate_get(patch);
    format.bitsPerSample = 32;

    sk_drwav_init_file_write(&wavout->wav, filename, &format, NULL);
    wavout->count = 0;

    gf_node_set_data(node, wavout);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &in, 0);
    return 0;
}

int sk_node_wavouts(sk_core *core, const char *filename)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param inL, inR;
    void *ud;
    struct wavout_n *wavout;
    sk_drwav_data_format format;

    rc = sk_param_get(core, &inR);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &inL);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct wavout_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    wavout = (struct wavout_n *)ud;

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_get_cable(node, 0, &wavout->inL);
    gf_node_get_cable(node, 1, &wavout->inR);


    format.container = sk_drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = 2;
    format.sampleRate = gf_patch_srate_get(patch);
    format.bitsPerSample = 32;

    sk_drwav_init_file_write(&wavout->wav, filename, &format, NULL);
    wavout->count = 0;

    gf_node_set_data(node, wavout);
    gf_node_set_compute(node, s_compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &inL, 0);
    sk_param_set(core, node, &inR, 1);
    return 0;
}
