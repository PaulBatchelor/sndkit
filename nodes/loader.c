#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

void sklil_load_sine(lil_t lil);
void sklil_load_wavout(lil_t lil);
void sklil_load_arith(lil_t lil);
void sklil_load_scale(lil_t lil);
void sklil_load_phasor(lil_t lil);
void sklil_load_rephasor(lil_t lil);
void sklil_load_tab(lil_t lil);
void sklil_load_gen(lil_t lil);
void sklil_load_osc(lil_t lil);
void sklil_load_reg(lil_t lil);
void sklil_load_buffer(lil_t lil);
void sklil_load_bigverb(lil_t lil);
void sklil_load_fmpair(lil_t lil);
void sklil_load_dcblocker(lil_t lil);
void sklil_load_smoother(lil_t lil);
void sklil_load_metro(lil_t lil);
void sklil_load_mtof(lil_t lil);
void sklil_load_expon(lil_t lil);
void sklil_load_rline(lil_t lil);
void sklil_load_vardelay(lil_t lil);
void sklil_load_expmap(lil_t lil);
void sklil_load_oscf(lil_t lil);
void sklil_load_phasewarp(lil_t lil);
void sklil_load_biramp(lil_t lil);
void sklil_load_modalres(lil_t lil);
void sklil_load_chorus(lil_t lil);
void sklil_load_bezier(lil_t lil);
void sklil_load_bitnoise(lil_t lil);
void sklil_load_chaosnoise(lil_t lil);
void sklil_load_peakeq(lil_t lil);
void sklil_load_valp1(lil_t lil);
void sklil_load_bitosc(lil_t lil);
void sklil_load_wavin(lil_t lil);
void sklil_load_env(lil_t lil);
void sklil_load_glottis(lil_t lil);
void sklil_load_tract(lil_t lil);
void sklil_load_blep(lil_t lil);
void sklil_load_phsclk(lil_t lil);
void sklil_load_vowel(lil_t lil);
void sklil_load_clkphs(lil_t lil);
void sklil_load_noise(lil_t lil);
void sklil_load_butterworth(lil_t lil);
void sklil_load_cable(lil_t lil);
void sklil_load_sparse(lil_t lil);
void sklil_load_softclip(lil_t lil);
void sklil_load_tseq(lil_t lil);
void sklil_load_thresh(lil_t lil);
void sklil_load_adsr(lil_t lil);
void sklil_load_tenv(lil_t lil);
void sklil_load_tick(lil_t lil);
void sklil_load_tgate(lil_t lil);
void sklil_load_dblin(lil_t lil);
void sklil_load_trand(lil_t lil);
void sklil_load_crossfade(lil_t lil);
void sklil_load_loadwav(lil_t lil);
void sklil_load_tsmp(lil_t lil);
void sklil_load_plotter(lil_t lil);
void sklil_load_qgliss(lil_t lil);
void sklil_load_tdiv(lil_t lil);
void sklil_load_shelf(lil_t lil);

void sklil_nodes(lil_t lil)
{
    sklil_load_sine(lil);
    sklil_load_wavout(lil);
    sklil_load_arith(lil);
    sklil_load_scale(lil);
    sklil_load_phasor(lil);
    sklil_load_rephasor(lil);
    sklil_load_tab(lil);
    sklil_load_gen(lil);
    sklil_load_osc(lil);
    sklil_load_reg(lil);
    sklil_load_buffer(lil);
    sklil_load_bigverb(lil);
    sklil_load_fmpair(lil);
    sklil_load_dcblocker(lil);
    sklil_load_smoother(lil);
    sklil_load_metro(lil);
    sklil_load_mtof(lil);
    sklil_load_expon(lil);
    sklil_load_rline(lil);
    sklil_load_vardelay(lil);
    sklil_load_expmap(lil);
    sklil_load_oscf(lil);
    sklil_load_phasewarp(lil);
    sklil_load_biramp(lil);
    sklil_load_modalres(lil);
    sklil_load_chorus(lil);
    sklil_load_bezier(lil);
    sklil_load_bitnoise(lil);
    sklil_load_chaosnoise(lil);
    sklil_load_peakeq(lil);
    sklil_load_valp1(lil);
    sklil_load_bitosc(lil);
    sklil_load_wavin(lil);
    sklil_load_env(lil);
    sklil_load_glottis(lil);
    sklil_load_tract(lil);
    sklil_load_blep(lil);
    sklil_load_phsclk(lil);
    sklil_load_vowel(lil);
    sklil_load_clkphs(lil);
    sklil_load_noise(lil);
    sklil_load_butterworth(lil);
    sklil_load_cable(lil);
    sklil_load_sparse(lil);
    sklil_load_softclip(lil);
    sklil_load_tseq(lil);
    sklil_load_thresh(lil);
    sklil_load_adsr(lil);
    sklil_load_tenv(lil);
    sklil_load_tick(lil);
    sklil_load_tgate(lil);
    sklil_load_dblin(lil);
    sklil_load_trand(lil);
    sklil_load_crossfade(lil);
    sklil_load_tsmp(lil);
    sklil_load_loadwav(lil);
    sklil_load_plotter(lil);
    sklil_load_qgliss(lil);
    sklil_load_tdiv(lil);
    sklil_load_shelf(lil);
}

static lil_value_t computes(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "computes", argc, 1);

    sk_core_computes(core, lil_to_double(argv[0]));
    return NULL;
}

static lil_value_t compute(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);


    if (argc > 0) {
        int nblks, b;
        nblks = lil_to_integer(argv[0]);
        for (b = 0; b < nblks; b++) {
            sk_core_compute(core);
        }
    } else {
        sk_core_compute(core);
    }
    return NULL;
}

static lil_value_t param(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "param", argc, 1);

    sklil_param(core, argv[0]);
    return NULL;
}

static lil_value_t regnxt(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int r;
    int rc;
    int start;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "regnxt", argc, 1);

    start = lil_to_integer(argv[0]);
    rc = sk_core_regnxt(core, start, &r);

    SKLIL_ERROR_CHECK(lil, rc, "regnxt failed.");

    return lil_alloc_integer(r);
}

static lil_value_t regclr(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int r;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "regclr", argc, 1);

    r = lil_to_integer(argv[0]);
    rc = sk_core_regclr(core, r);

    SKLIL_ERROR_CHECK(lil, rc, "regclr failed.");

    return NULL;
}

static lil_value_t regmrk(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int r;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "regmrk", argc, 1);

    r = lil_to_integer(argv[0]);
    rc = sk_core_regmrk(core, r);

    SKLIL_ERROR_CHECK(lil, rc, "regmrk failed.");

    return NULL;
}

static lil_value_t l_srand(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int val;

    core = lil_get_data(lil);

    if (argc > 0) {
        val = lil_to_integer(argv[0]);
    } else {
        val = time(NULL);
    }

    sk_core_srand(core, val);


    return NULL;
}

static lil_value_t l_rand(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);
    return lil_alloc_integer(sk_core_rand(core));
}

static lil_value_t l_randf(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);
    return lil_alloc_double(sk_core_randf(core));
}

static lil_value_t l_grab(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    const char *key;
    int rc;

    SKLIL_ARITY_CHECK(lil, "grab", argc, 1);

    core = lil_get_data(lil);

    key = lil_to_string(argv[0]);

    rc = sk_core_grab(core, key, strlen(key));

    if (rc == 1) {
        lil_set_error(lil, "Could not find key.");
    } else if (rc == 2) {
        lil_set_error(lil, "Could not push to stack.");
    }

    return NULL;
}

static lil_value_t l_blkset(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int sz;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "blkset", argc, 1);

    sz = lil_to_integer(argv[0]);
    rc = sk_core_blkset(core, sz);

    SKLIL_ERROR_CHECK(lil, rc, "blkset failed.");

    return NULL;
}

static lil_value_t l_stackpos(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;

    core = lil_get_data(lil);

    return lil_alloc_integer(sk_core_stackpos(core));
}

static lil_value_t l_unholdall(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    gf_patch *patch;
    gf_bufferpool *pool;

    core = lil_get_data(lil);
    patch = sk_core_patch(core);
    pool = gf_patch_pool(patch);

    /* usually, when this is called, we also mean to
     * reset the whole pool as well
     */
    gf_bufferpool_reset(pool);


    /* discarding error because error is overkill */
    gf_bufferpool_unholdu_all(pool);

    return NULL;
}

static lil_value_t l_pop(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    SKFLT x;
    int rc;

    core = lil_get_data(lil);

    x = 0;
    rc = sk_param_get_constant(core, &x);
    SKLIL_ERROR_CHECK(lil, rc, "value is not a constant");

    return lil_alloc_double(x);
}

static lil_value_t l_del(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    const char *key;
    int rc;

    core = lil_get_data(lil);
    SKLIL_ARITY_CHECK(lil, "del", argc, 1);
    key = lil_to_string(argv[0]);

    rc = sk_dict_remove(sk_core_dict(core), key, strlen(key));
    SKLIL_ERROR_CHECK(lil, rc, "no value to remove");

    return NULL;
}

void lil_get_cmds(lil_t lil, lil_func_t **cmd, size_t *ncmd);
const char *lil_cmd_name(lil_func_t cmd);

static lil_value_t l_lscmds(lil_t lil, size_t argc, lil_value_t *argv)
{
    size_t c, ncmd;
    lil_func_t *cmd;
    c = ncmd = 0;

    lil_get_cmds(lil, &cmd, &ncmd);

    for (c = 0; c < ncmd; c++) {
        printf("%s\n", lil_cmd_name(cmd[c]));
    }

    return NULL;
}

void sklil_loader(lil_t lil)
{
    sk_core *core;
    core = sk_core_new(44100);
    lil_set_data(lil, core);
    sklil_nodes(lil);
    lil_register(lil, "compute", compute);
    lil_register(lil, "computes", computes);
    lil_register(lil, "param", param);
    lil_register(lil, "regnxt", regnxt);
    lil_register(lil, "regmrk", regmrk);
    lil_register(lil, "regclr", regclr);
    lil_register(lil, "srand", l_srand);
    lil_register(lil, "rand", l_rand);
    lil_register(lil, "randf", l_randf);
    lil_register(lil, "grab", l_grab);
    lil_register(lil, "blkset", l_blkset);
    lil_register(lil, "stkpos", l_stackpos);
    lil_register(lil, "unholdall", l_unholdall);
    lil_register(lil, "pop", l_pop);
    lil_register(lil, "del", l_del);
    lil_register(lil, "lscmds", l_lscmds);
}

void sklil_clean(lil_t lil)
{
    sk_core *core;
    core = lil_get_data(lil);
    sk_core_del(core);
}
