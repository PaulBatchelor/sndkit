#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "patchwerk.h"
#include "core.h"
#define SK_OSCF_PRIV
#include "dsp/oscf.h"

struct oscf_n {
    pw_cable *freq;
    pw_cable *out;
    sk_oscf oscf;
};

static void oscf_compute(pw_node *node)
{
    int blksize;
    int n;
    struct oscf_n *oscf;

    blksize = pw_node_blksize(node);

    oscf = (struct oscf_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, freq;
        freq = pw_cable_get(oscf->freq, n);

        sk_oscf_freq(&oscf->oscf, freq);
        out = sk_oscf_tick(&oscf->oscf);

        pw_cable_set(oscf->out, n, out);
    }
}

/* hijack 'freq' cable to be external phasor */
static void oscfext_compute(pw_node *node)
{
    int blksize;
    int n;
    struct oscf_n *oscf;

    blksize = pw_node_blksize(node);

    oscf = (struct oscf_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT out, phs;
        phs = pw_cable_get(oscf->freq, n);

        out = sk_oscf_tick_extphs(&oscf->oscf, phs);

        pw_cable_set(oscf->out, n, out);
    }
}

static void destroy(pw_node *node)
{
    pw_patch *patch;
    int rc;
    void *ud;
    rc = pw_node_get_patch(node, &patch);
    if (rc != PW_OK) return;
    pw_node_cables_free(node);
    ud = pw_node_get_data(node);
    pw_memory_free(patch, &ud);
}

int sk_node_oscf(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct oscf_n *oscf;
    int sr;
    sk_table *tab;
    SKFLT iphs;

    rc = sk_param_get_constant(core, &iphs);
    SK_ERROR_CHECK(rc);

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct oscf_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    oscf = (struct oscf_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_oscf_init(&oscf->oscf, sr,
                 sk_table_data(tab),
                 sk_table_size(tab),
                 iphs);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &oscf->freq);
    pw_node_get_cable(node, 1, &oscf->out);

    pw_node_set_data(node, oscf);
    pw_node_set_compute(node, oscf_compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}

int sk_node_oscfext(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
    int rc;
    sk_param freq;
    void *ud;
    struct oscf_n *oscf;
    int sr;
    sk_table *tab;

    rc = sk_param_get(core, &freq);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = pw_memory_alloc(patch, sizeof(struct oscf_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    oscf = (struct oscf_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_oscf_init(&oscf->oscf, sr,
                 sk_table_data(tab),
                 sk_table_size(tab),
                 0);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 2);
    SK_PW_ERROR_CHECK(rc);

    pw_node_set_block(node, 1);

    pw_node_get_cable(node, 0, &oscf->freq);
    pw_node_get_cable(node, 1, &oscf->out);

    pw_node_set_data(node, oscf);
    pw_node_set_compute(node, oscfext_compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &freq, 0);
    sk_param_out(core, node, 1);
    return 0;
}
