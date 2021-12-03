#include "../kissfft/kiss_fftr.h"
#include "../../graforge/graforge.h"
#include "../../core.h"

int sk_mags(sk_core *core)
{
    kiss_fftr_cfg fft;
    kiss_fft_cpx *tmp;
    sk_table *ft;
    sk_table *mags;
    int magsz;
    int n;
    int ftsize;
    SKFLT *ftab;
    SKFLT *mtab;
    int rc;

    rc = sk_core_table_pop(core, &ft);
    SK_ERROR_CHECK(rc);

    ftsize = sk_table_size(ft);
    ftab = sk_table_data(ft);
    magsz = ftsize / 2;

    rc = sk_core_table_new(core, magsz);
    SK_ERROR_CHECK(rc);

    rc = sk_core_table_pop(core, &mags);
    SK_ERROR_CHECK(rc);

    mtab = sk_table_data(mags);

    fft = kiss_fftr_alloc(ftsize, 0, NULL, NULL);
    tmp = calloc(1, sizeof(kiss_fft_cpx) * ftsize);

    kiss_fftr(fft, ftab, tmp);

    for (n = 0; n < magsz; n++) {
        mtab[n] = sqrt(tmp[n].r*tmp[n].r +
                       tmp[n].i*tmp[n].i);
    }

    kiss_fftr_free(fft);
    KISS_FFT_FREE(tmp);
    rc = sk_core_table_push(core, mags);
    SK_ERROR_CHECK(rc);

    return 0;
}
