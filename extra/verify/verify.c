#include <stdlib.h>
#include <stdio.h>
#include "../../graforge/graforge.h"
#include "../../core.h"
#include "md5.h"

static char nibble(int x)
{
    char out;

    out = '0';
    x &= 0xf;

    if (x >= 0x0 && x <= 0x9) {
        out = '0' + x;
    } else if (x >= 0xa && x <= 0xf) {
        out = 'a' + (x - 0xa);
    }

    return out;
}

int sk_verify(sk_core *core, char *out)
{
    md5_state_t state;
    md5_byte_t digest[16];
    SKFLT *buf;
    unsigned long nsmps;
    unsigned long n;
    int i;
    sk_param in;
    int rc;
    gf_patch *patch;
    gf_cable *c;

    patch = sk_core_patch(core);

    if (gf_patch_blksize(patch) != 64) {
        fprintf(stderr, "block size should be 64\n");
        return 1;
    }

    rc = sk_param_get_cable(core, &in);
    SK_ERROR_CHECK(rc);

    c = sk_param_cable(&in);

    SK_ERROR_CHECK(rc);

    nsmps = 64 * 3446;

    buf = calloc(1, nsmps * sizeof(SKFLT));

    for (n = 0; n < 3446; n++) {
        sk_core_compute(core);
        for (i = 0; i < 64; i++) {
            buf[n*64 + i] = gf_cable_get(c, i);
        }
    }

    md5_init(&state);

    md5_append(&state,
               (const md5_byte_t *)buf,
               sizeof(SKFLT) * nsmps);
    md5_finish(&state, digest);

    for (n = 0; n < 16; n++) {
        out[2*n + 1] = nibble(digest[n]);
        out[2*n] = nibble(digest[n]>>4);
    }

    out[32] = '\0';

    free(buf);
    return 0;
}
