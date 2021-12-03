#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "graforge.h"
#include "core.h"
#define SK_VOWEL_PRIV
#include "dsp/vowel.h"

struct vowelmorph_n {
    gf_cable *in;
    gf_cable *pos;
    gf_cable *voice;
    gf_cable *out;
    sk_vowel vowel;
    sk_vowel_formant phoneme[5];
    sk_vowel_formant tmp[5];
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct vowelmorph_n *vowelmorph;

    blksize = gf_node_blksize(node);

    vowelmorph = (struct vowelmorph_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, pos, voice, out;
        in = gf_cable_get(vowelmorph->in, n);
        pos = gf_cable_get(vowelmorph->pos, n);
        voice = gf_cable_get(vowelmorph->voice, n);

        sk_vowel_morph(vowelmorph->phoneme, vowelmorph->tmp, 5, pos, voice);
        sk_vowel_set_phoneme(&vowelmorph->vowel, vowelmorph->phoneme, 5);
        out = sk_vowel_tick(&vowelmorph->vowel, in);
        gf_cable_set(vowelmorph->out, n, out);
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

int gf_node_vowelmorph(gf_node *node)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct vowelmorph_n *vowelmorph;
    int i;
    int sr;

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct vowelmorph_n), &ud);
    if (rc != GF_OK) return rc;

    vowelmorph = (struct vowelmorph_n *)ud;

    sr = gf_patch_srate_get(patch);

    sk_vowel_init(&vowelmorph->vowel, sr);

    for (i = 0; i < 5; i++) {
        vowelmorph->phoneme[i] = sk_vowel_bass_a[i];
    }

    rc = gf_node_cables_alloc(node, 4);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 3);

    gf_node_get_cable(node, 0, &vowelmorph->in);
    gf_node_get_cable(node, 1, &vowelmorph->pos);
    gf_node_get_cable(node, 2, &vowelmorph->voice);
    gf_node_get_cable(node, 3, &vowelmorph->out);

    gf_node_set_data(node, vowelmorph);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_vowelmorph(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;
    sk_param pos;
    sk_param voice;

    rc = sk_param_get(core, &voice);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &pos);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_vowelmorph(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_set(core, node, &pos, 1);
    sk_param_set(core, node, &voice, 2);
    sk_param_out(core, node, 3);

    return 0;
}

struct vowel_n {
    gf_cable *in;
    gf_cable *out;
    sk_vowel_withphoneme *vowel;
};

static void compute_vowel(gf_node *node)
{
    int blksize;
    int n;
    struct vowel_n *vd;
    sk_vowel_withphoneme *vow;

    vd = (struct vowel_n *)gf_node_get_data(node);
    vow = vd->vowel;
    blksize = gf_node_blksize(node);

    for (n = 0; n < blksize; n++) {
        GFFLT in, out;
        in = gf_cable_get(vd->in, n);
        sk_vowel_set_phoneme(&vow->vowel, vow->phoneme, 5);
        out = sk_vowel_tick(&vow->vowel, in);
        gf_cable_set(vd->out, n, out);
    }
}

int gf_node_vowel(gf_node *node, sk_vowel_withphoneme *vow)
{
    gf_patch *patch;
    int rc;
    void *ud;
    struct vowel_n *vowel;
    int i;

    rc = gf_node_get_patch(node, &patch);
    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct vowel_n), &ud);
    if (rc != GF_OK) return rc;

    vowel = (struct vowel_n *)ud;

    vowel->vowel = vow;

    for (i = 0; i < 5; i++) {
        vow->phoneme[i] = sk_vowel_bass_a[i];
    }

    rc = gf_node_cables_alloc(node, 2);
    SK_GF_ERROR_CHECK(rc);

    gf_node_set_block(node, 1);

    gf_node_get_cable(node, 0, &vowel->in);
    gf_node_get_cable(node, 1, &vowel->out);

    gf_node_set_data(node, vowel);
    gf_node_set_compute(node, compute_vowel);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_vowel(sk_core *core, sk_vowel_withphoneme *vow)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param in;

    rc = sk_param_get(core, &in);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_vowel(node, vow);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &in, 0);
    sk_param_out(core, node, 1);

    return 0;
}

static void cleanup_vowel(gf_pointer *p)
{
    sk_vowel_withphoneme *vow;

    vow = (sk_vowel_withphoneme *)gf_pointer_data(p);

    free(vow);
}

sk_vowel_withphoneme * sk_vowel_alloc(gf_patch *patch)
{
    int i;
    sk_vowel_withphoneme *vow;

    vow = malloc(sizeof(sk_vowel_withphoneme));
    sk_vowel_init(&vow->vowel, gf_patch_srate_get(patch));

    for (i = 0; i < 5; i++) {
        vow->phoneme[i] = sk_vowel_bass_a[i];
    }

    gf_patch_append_userdata(patch, cleanup_vowel, vow);
    return vow;
}

sk_vowel_withphoneme * sk_node_vowelnew(sk_core *core)
{
    gf_patch *patch;
    patch = sk_core_patch(core);
    return sk_vowel_alloc(patch);
}
