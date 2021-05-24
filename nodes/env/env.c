#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "patchwerk.h"
#include "core.h"
#define SK_ENV_PRIV
#include "dsp/env.h"

struct env_n {
    pw_cable *trig;
    pw_cable *atk;
    pw_cable *hold;
    pw_cable *rel;
    pw_cable *out;
    sk_env env;
};


static void compute(pw_node *node)
{
    int blksize;
    int n;
    struct env_n *env;

    blksize = pw_node_blksize(node);

    env = (struct env_n *)pw_node_get_data(node);

    for (n = 0; n < blksize; n++) {
        PWFLT trig, atk, hold, rel, out;

        trig = pw_cable_get(env->trig, n);
        atk = pw_cable_get(env->atk, n);
        sk_env_attack(&env->env, atk);
        hold = pw_cable_get(env->hold, n);
        sk_env_hold(&env->env, hold);
        rel = pw_cable_get(env->rel, n);
        sk_env_release(&env->env, rel);

        out = sk_env_tick(&env->env, trig);
        pw_cable_set(env->out, n, out);
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

int sk_node_env(sk_core *core)
{
    pw_patch *patch;
    pw_node *node;
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

    rc = pw_memory_alloc(patch, sizeof(struct env_n), &ud);
    SK_PW_ERROR_CHECK(rc);
    env = (struct env_n *)ud;

    sr = pw_patch_srate_get(patch);
    sk_env_init(&env->env, sr);

    rc = pw_patch_new_node(patch, &node);
    SK_PW_ERROR_CHECK(rc);

    rc = pw_node_cables_alloc(node, 5);
    SK_PW_ERROR_CHECK(rc);
    pw_node_set_block(node, 4);


    pw_node_get_cable(node, 0, &env->trig);
    pw_node_get_cable(node, 1, &env->atk);
    pw_node_get_cable(node, 2, &env->hold);
    pw_node_get_cable(node, 3, &env->rel);
    pw_node_get_cable(node, 4, &env->out);

    pw_node_set_data(node, env);
    pw_node_set_compute(node, compute);
    pw_node_set_destroy(node, destroy);

    sk_param_set(core, node, &trig, 0);
    sk_param_set(core, node, &atk, 1);
    sk_param_set(core, node, &hold, 2);
    sk_param_set(core, node, &rel, 3);
    sk_param_out(core, node, 4);
    return 0;
}
