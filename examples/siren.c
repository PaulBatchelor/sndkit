#include <math.h>
#include <stdio.h>
#include "graforge.h"
#include "core.h"
#include "sknodes.h"

int patch(sk_core *core)
{
    /* warning: no error checking */

    sk_core_constant(core, 0.1);
    sk_core_constant(core, 1);
    sk_node_sine(core);
    sk_core_hold(core);
    sk_core_regset(core, 0);

    sk_core_table_new(core, 8192);
    sk_node_gensine(core);

    sk_core_regget(core, 0);
    sk_core_constant(core, 100);
    sk_core_constant(core, 600);
    sk_node_biscale(core);

    sk_core_constant(core, 1);
    sk_core_constant(core, 1);

    sk_core_regget(core, 0);
    sk_core_constant(core, 0.1);
    sk_core_constant(core, 2);
    sk_node_biscale(core);

    sk_core_constant(core, 0);

    sk_node_fmpair(core);

    sk_core_constant(core, 0.5);
    sk_node_mul(core);

    sk_core_dup(core);
    sk_core_dup(core);

    sk_core_constant(core, 0.97);
    sk_core_constant(core, 10000);
    sk_node_bigverb(core);
    sk_core_drop(core);
    sk_core_constant(core, 0.1);
    sk_node_mul(core);
    sk_node_dcblocker(core);

    sk_node_add(core);

    sk_node_wavout(core, "siren.wav");

    sk_core_regget(core, 0);
    sk_core_unhold(core);

    return 0;
}

int main(int argc, char *argv[])
{
    sk_core *core;
    unsigned int n;
    unsigned int nblocks;
    int rc;

    core = sk_core_new(44100);

    rc = patch(core);

    if (rc) {
        fprintf(stderr, "Error code %d\n", rc);
        goto clean;
    }

    nblocks = sk_core_seconds_to_blocks(core, 10);

    for (n = 0; n < nblocks; n++) {
        sk_core_compute(core);
    }

    clean:
    sk_core_del(core);
    return rc;
}
