#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "graforge.h"
#include "core.h"
#define SK_ENV_PRIV
#include "dsp/env.h"

struct env_n {
    gf_cable *trig;
    gf_cable *atk;
    gf_cable *hold;
    gf_cable *rel;
    gf_cable *out;
    sk_env env;
};


static void compute(gf_node *node)
{
    int blksize;
    int n;
    struct env_n *env;

    blksize = gf_node_blksize(node);

    env = (struct env_n *)gf_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        GFFLT trig, atk, hold, rel, out;

        trig = gf_cable_get(env->trig, n);
        atk = gf_cable_get(env->atk, n);
        sk_env_attack(&env->env, atk);
        hold = gf_cable_get(env->hold, n);
        sk_env_hold(&env->env, hold);
        rel = gf_cable_get(env->rel, n);
        sk_env_release(&env->env, rel);

        out = sk_env_tick(&env->env, trig);
        gf_cable_set(env->out, n, out);
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

int sk_node_env(sk_core *core)
{
    gf_patch *patch;
    gf_node *node;
    int rc;
    sk_param trig;
    sk_param atk;
    sk_param hold;
    sk_param rel;
    void *ud;
    struct env_n *env;
    int sr;

    rc = sk_param_get(core, &rel);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &hold);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &atk);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get(core, &trig);
    SK_ERROR_CHECK(rc);

    patch = sk_core_patch(core);

    rc = gf_memory_alloc(patch, sizeof(struct env_n), &ud);
    SK_GF_ERROR_CHECK(rc);
    env = (struct env_n *)ud;

    sr = gf_patch_srate_get(patch);
    sk_env_init(&env->env, sr);

    rc = gf_patch_new_node(patch, &node);
    SK_GF_ERROR_CHECK(rc);

    rc = gf_node_cables_alloc(node, 5);
    SK_GF_ERROR_CHECK(rc);
    gf_node_set_block(node, 4);


    gf_node_get_cable(node, 0, &env->trig);
    gf_node_get_cable(node, 1, &env->atk);
    gf_node_get_cable(node, 2, &env->hold);
    gf_node_get_cable(node, 3, &env->rel);
    gf_node_get_cable(node, 4, &env->out);

    gf_node_set_data(node, env);
    gf_node_set_compute(node, compute);
    gf_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &atk, 1);
    sk_param_set(core, node, &hold, 2);
    sk_param_set(core, node, &rel, 3);
    sk_param_out(core, node, 4);
    return 0;
}
