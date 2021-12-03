/*
 * Talkbox
 *
 * This module is ported from the mdaTalkbox plugin by Paul Kellet
 * (maxim digital audio), placed under an MIT license.
 *
 * More information on his plugins and the original code can be found here:
 *
 * http://mda.smartelectronix.com/
 *
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "graforge.h"
#include "core.h"

#include "talkbox.h"

struct sk_talkbox {
    SKFLT quality;
    SKFLT d0, d1, d2, d3, d4;
    SKFLT u0, u1, u2, u3, u4;
    SKFLT FX;
    SKFLT emphasis;
    SKFLT car0[SK_TALKBOX_BUFMAX];
    SKFLT car1[SK_TALKBOX_BUFMAX];
    SKFLT window[SK_TALKBOX_BUFMAX];
    SKFLT buf0[SK_TALKBOX_BUFMAX];
    SKFLT buf1[SK_TALKBOX_BUFMAX];
    uint32_t K, N, O, pos;
    int sr;
};

#ifndef TWO_PI
#define TWO_PI 6.28318530717958647692528676655901
#endif

#define ORD_MAX 50

static void lpc_durbin(SKFLT *r, int p, float *k, float *g)
{
  int i, j;
  SKFLT a[ORD_MAX], at[ORD_MAX], e=r[0];

  for (i=0; i<=p; i++) a[i] = at[i] = 0.0f;

  for(i=1; i<=p; i++) {
    k[i] = -r[i];

    for(j=1; j<i; j++) {
      at[j] = a[j];
      k[i] -= a[j] * r[i-j];
    }

    if (fabs(e) < 1.0e-20f) {
        e = 0.0f;  break;
    }

    k[i] /= e;

    a[i] = k[i];
    for(j=1; j<i; j++) a[j] = at[j] + k[i] * at[i-j];

    e *= 1.0f - k[i] * k[i];
  }

  if(e < 1.0e-20f) e = 0.0f;
  *g = (float)sqrt(e);
}

static void lpc(float *buf, float *car, uint32_t n, uint32_t o)
{
    SKFLT z[ORD_MAX], r[ORD_MAX], k[ORD_MAX], G, x;
    uint32_t i, j, nn=n;
    SKFLT min;

    /* buf[] is already emphasized and windowed */
    for(j=0; j<=o; j++, nn--) {
        z[j] = r[j] = 0.0f;
        for(i=0; i<nn; i++) r[j] += buf[i] * buf[i+j]; /* autocorrelation */
    }

    r[0] *= 1.001f;  /* stability fix */

    min = 0.00001f;
    if(r[0] < min) { for(i=0; i<n; i++) buf[i] = 0.0f; return; }

    lpc_durbin(r, o, k, &G);  /* calc reflection coeffs */

    for (i=1; i<=o; i++) {
        if(k[i] > 0.995f) k[i] = 0.995f; else if(k[i] < -0.995f) k[i] = -.995f;
    }

    for (i=0; i<n; i++) {
        x = G * car[i];
        /* lattice filter */
        for (j=o; j>0; j--) {
            x -= k[j] * z[j-1];
            z[j] = z[j-1] + k[j] * x;
        }
        buf[i] = z[0] = x;  /* output buf[] will be windowed elsewhere */
    }
}

void sk_talkbox_init(sk_talkbox *t, int sr)
{
    uint32_t n;

    t->quality = 1.f;
    t->N = 1;
    t->K = 0;
    t->sr = sr;

    n = (uint32_t)(0.01633f * sr);
    if (n > SK_TALKBOX_BUFMAX) n = SK_TALKBOX_BUFMAX;

    /* calculate hanning window */
    if (n != t->N) {
        SKFLT dp;
        SKFLT pos;
        t->N = n;
        dp = TWO_PI / (SKFLT)t->N;
        pos = 0.0f;
        for (n=0; n < t->N; n++) {
            t->window[n] = 0.5f - 0.5f * (SKFLT)cos(pos);
            pos += dp;
        }
    }

    /* zero out variables and buffers */
    t->pos = t->K = 0;
    t->emphasis = 0.0f;
    t->FX = 0;

    t->u0 = t->u1 = t->u2 = t->u3 = t->u4 = 0.0f;
    t->d0 = t->d1 = t->d2 = t->d3 = t->d4 = 0.0f;

    memset(t->buf0, 0, SK_TALKBOX_BUFMAX * sizeof(SKFLT));
    memset(t->buf1, 0, SK_TALKBOX_BUFMAX * sizeof(SKFLT));
    memset(t->car0, 0, SK_TALKBOX_BUFMAX * sizeof(SKFLT));
    memset(t->car1, 0, SK_TALKBOX_BUFMAX * sizeof(SKFLT));
}

SKFLT sk_talkbox_tick(sk_talkbox *t, SKFLT src, SKFLT exc)
{
    int32_t p0, p1;
    SKFLT e, w, o, x, fx;
    SKFLT p, q, h0, h1;
    SKFLT den;
    SKFLT out;

    fx = t->FX;
    p0 = t->pos;
    p1 = (t->pos + t->N/2) % t->N;
    h0=0.3f;
    h1=0.77f;


    e = t->emphasis;
    t->O = (uint32_t)((0.0001f + 0.0004f * t->quality) * t->sr);

    o = src;
    x = exc;

    p = t->d0 + h0 * x;
    t->d0 = t->d1;
    t->d1 = x - h0 * p;

    q = t->d2 + h1 * t->d4;
    t->d2 = t->d3;
    t->d3 = t->d4 - h1 * q;

    t->d4 = x;

    x = p + q;

    if (t->K++) {
        t->K = 0;

        /* carrier input */
        t->car0[p0] = t->car1[p1] = x;

        /* 6dB/oct pre-emphasis */
        x = o - e;  e = o;

        /* 50% overlapping hanning windows */
        w = t->window[p0];
        fx = t->buf0[p0] * w;
        t->buf0[p0] = x * w;

        if (++p0 >= t->N) {
            lpc(t->buf0, t->car0, t->N, t->O);
            p0 = 0;
        }

        w = 1.0f - w;
        fx += t->buf1[p1] * w;
        t->buf1[p1] = x * w;

        if (++p1 >= t->N) {
            lpc(t->buf1, t->car1, t->N, t->O);  p1 = 0;
        }
    }

    p = t->u0 + h0 * fx;
    t->u0 = t->u1;
    t->u1 = fx - h0 * p;

    q = t->u2 + h1 * t->u4;
    t->u2 = t->u3;
    t->u3 = t->u4 - h1 * q;

    t->u4 = fx;
    x = p + q;

    o = x * 0.5;
    out = o;
    t->emphasis = e;
    t->pos = p0;
    t->FX = fx;

    den = 1.0e-10f;

    /* anti-denormal */
    if(fabs(t->d0) < den) t->d0 = 0.0f;
    if(fabs(t->d1) < den) t->d1 = 0.0f;
    if(fabs(t->d2) < den) t->d2 = 0.0f;
    if(fabs(t->d3) < den) t->d3 = 0.0f;
    if(fabs(t->u0) < den) t->u0 = 0.0f;
    if(fabs(t->u1) < den) t->u1 = 0.0f;
    if(fabs(t->u2) < den) t->u2 = 0.0f;
    if(fabs(t->u3) < den) t->u3 = 0.0f;

    return out;
}

struct talkbox_n {
    gf_cable *src;
    gf_cable *exc;
    gf_cable *quality;
    gf_cable *out;
    sk_talkbox tb;
};

static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct talkbox_n *tb;

    blksize = gf_node_blksize(node);

    tb = (struct talkbox_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT src, exc, quality, out;

        src = gf_cable_get(tb->src, n);
        exc = gf_cable_get(tb->exc, n);
        quality = gf_cable_get(tb->quality, n);
        tb->tb.quality = quality;
        out = sk_talkbox_tick(&tb->tb, src, exc);

        gf_cable_set(tb->out, n, out);
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

int gf_node_talkbox(gf_node *node)
{
    int rc;
    gf_patch *patch;
    struct talkbox_n *tb;
    int sr;
    void *ud;

    rc = gf_node_get_patch(node, &patch);

    if (rc != GF_OK) return rc;

    rc = gf_memory_alloc(patch, sizeof(struct talkbox_n), &ud);
    if (rc != GF_OK) return rc;

    tb = (struct talkbox_n *)ud;
    sr = gf_patch_srate_get(patch);

    sk_talkbox_init(&tb->tb, sr);

    rc = gf_node_cables_alloc(node, 4);

    if (rc != GF_OK) return rc;

    gf_node_get_cable(node, 0, &tb->src);
    gf_node_get_cable(node, 1, &tb->exc);
    gf_node_get_cable(node, 2, &tb->quality);
    gf_node_set_block(node, 3);
    gf_node_get_cable(node, 3, &tb->out);


    gf_node_set_data(node, tb);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    return GF_OK;
}

int sk_node_talkbox(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param src;
    sk_param exc;
    sk_param quality;

    rc = sk_param_get(core, &quality);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &exc);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &src);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_talkbox(node);
    SK_GF_ERROR_CHECK(rc);

    sk_param_set(core, node, &src, 0);
    sk_param_set(core, node, &exc, 1);
    sk_param_set(core, node, &quality, 2);
    sk_param_out(core, node, 3);
    return 0;
}
