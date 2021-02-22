#include <math.h>
#include <stdio.h>
#include "patchwerk.h"
#include "core.h"
#include "nodes/wavout.h"
#include "nodes/sine.h"

int main(int argc, char *argv[])
{
    sk_core *core;
    unsigned int n;
    unsigned int nblocks;

    core = sk_core_new(sr);

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
