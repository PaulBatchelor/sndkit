#include <stdlib.h>
#include <stdio.h>
#include "lil/lil.h"
#include "patchwerk/patchwerk.h"
#include "core.h"
#include "sklil.h"

void sklil_load_sine(lil_t lil);
void sklil_load_wavout(lil_t lil);
void sklil_load_arith(lil_t lil);
void sklil_load_scale(lil_t lil);
void sklil_load_phasor(lil_t lil);
void sklil_load_rephasor(lil_t lil);

void sklil_nodes(lil_t lil)
{
    sklil_load_sine(lil);
    sklil_load_wavout(lil);
    sklil_load_arith(lil);
    sklil_load_scale(lil);
    sklil_load_phasor(lil);
    sklil_load_rephasor(lil);
}

static lil_value_t computes(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "computes", argc, 1);

    sk_core_computes(core, lil_to_double(argv[0]));
    return NULL;
}

void sklil_loader(lil_t lil)
{
    sk_core *core;
    core = sk_core_new(44100);
    lil_set_data(lil, core);
    sklil_nodes(lil);
    lil_register(lil, "computes", computes);
}

void sklil_clean(lil_t lil)
{
    sk_core *core;
    core = lil_get_data(lil);
    sk_core_del(core);
}
