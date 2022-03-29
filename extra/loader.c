#include <stdlib.h>
#include <stdio.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

void sklil_load_verbity(lil_t lil);
void sklil_load_padsynth(lil_t lil);
void sklil_load_mags(lil_t lil);
void sklil_load_talkbox(lil_t lil);
void sklil_load_verify(lil_t lil);
void sklil_load_brown(lil_t lil);

void sklil_extra(lil_t lil)
{
    sklil_load_verbity(lil);
    sklil_load_padsynth(lil);
    sklil_load_mags(lil);
    sklil_load_talkbox(lil);
    sklil_load_verify(lil);
    sklil_load_brown(lil);
}

void sklil_loader_withextra(lil_t lil)
{
    sklil_loader(lil);
    sklil_extra(lil);
}
