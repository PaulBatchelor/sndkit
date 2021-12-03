#include <math.h>
#include <stdio.h>
#include "graforge.h"
#include "core.h"
#include "sknodes.h"

int main(int argc, char *argv[])
{
    sk_core *core;
    unsigned int n;
    unsigned int nblocks;

    core = sk_core_new(44100);

    nblocks = sk_core_seconds_to_blocks(core, 10);

    sk_core_constant(core, 440);
    sk_core_constant(core, 0.5);
    sk_node_sine(core);

    sk_node_wavout(core, "test.wav");

    for (n = 0; n < nblocks; n++) {
        sk_core_compute(core);
    }

    sk_core_del(core);
    return 0;
}
