#include <math.h>
#include <stdio.h>
#include "graforge.h"
#include "core.h"
#include "sknodes.h"

int patch(sk_core *core)
{
    int rc;

    /* paranoid error checking */

    rc = sk_core_constant(core, 5);
    SK_ERROR_CHECK(rc);
    rc = sk_core_constant(core, 40);
    SK_ERROR_CHECK(rc);
    rc = sk_node_sine(core);
    SK_ERROR_CHECK(rc);

    rc = sk_core_constant(core, 440);
    SK_ERROR_CHECK(rc);
    rc = sk_node_add(core);
    SK_ERROR_CHECK(rc);

    rc = sk_core_constant(core, 0.1);
    SK_ERROR_CHECK(rc);
    rc = sk_node_sine(core);
    SK_ERROR_CHECK(rc);
    rc = sk_node_wavout(core, "test.wav");
    SK_ERROR_CHECK(rc);

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
