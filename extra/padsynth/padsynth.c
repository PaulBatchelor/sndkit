/*
    Example implementation of the PADsynth basic algorithm
    By: Nasca O. Paul, Tg. Mures, Romania

    Ported to C by Paul Batchelor

    This implementation and the algorithm are released under Public Domain
    Feel free to use it into your projects or your products ;-)

    This implementation is tested under GCC/Linux, but it's
    very easy to port to other compiler/OS.
*/

#include <stdlib.h>
#include <math.h>
#include "../../graforge/graforge.h"
#include "../../core.h"

#include "../kissfft/kiss_fftr.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

/* This is the profile of one harmonic
   In this case is a Gaussian distribution (e^(-x^2))
   The amplitude is divided by the bandwidth to ensure that the harmonic
   keeps the same amplitude regardless of the bandwidth */

static SKFLT profile(SKFLT fi, SKFLT bwi)
{
    SKFLT x =fi/bwi;
    x *= x;

    /*
     * this avoids computing the e^(-x^2) where
     * it's results are very close to zero
     */

    if (x>14.71280603) return 0.0;

    return exp(-x)/bwi;
}

static void apply_ifft(int N, SKFLT *freq_amp,
                       SKFLT *freq_phase, SKFLT *smp)
{
    int i;
    kiss_fftr_cfg ifft;
    kiss_fft_cpx *fftfreqs;

    ifft = kiss_fftr_alloc(N, 1, NULL, NULL);
    fftfreqs = malloc(sizeof(kiss_fft_cpx) * N/2);

    for (i = 0; i < N/2; i++) {
        fftfreqs[i].r = freq_amp[i]*cos(freq_phase[i]);
        fftfreqs[i].i = freq_amp[i]*sin(freq_phase[i]);
    };

    kiss_fftri(ifft, fftfreqs, smp);

    free(fftfreqs);
    kiss_fftr_free(ifft);
}

/*
    Simple normalization function. It normalizes the sound to 1/sqrt(2)
*/

static void normalize(int N, SKFLT *smp)
{
    int i;
    SKFLT max=0.0;
    for (i = 0; i < N;i++) {
        if (fabs(smp[i]) > max) max = fabs(smp[i]);
    }
    if (max < 1e-5) max = 1e-5;
    for (i = 0; i < N; i++) smp[i] /= max*1.4142;
}


void sk_padsynth_dsp(sk_core *core, sk_table *ps,
                     sk_table *amps,
                     SKFLT f,
                     SKFLT bw)
{

    int i, nh;
    int N;
    int number_harmonics;
    SKFLT *A;
    SKFLT *smp;

    SKFLT *freq_amp;
    SKFLT *freq_phase;


    N = sk_table_size(ps);
    number_harmonics = sk_table_size(amps);
    A = sk_table_data(amps);
    smp = sk_table_data(ps);

    freq_amp = malloc((N / 2) * sizeof(SKFLT));
    freq_phase = malloc((N / 2) * sizeof(SKFLT));

    for (i = 0; i < N/2; i++) freq_amp[i]=0.0;

    for (nh=1; nh < number_harmonics; nh++) {
        SKFLT bw_Hz;
        SKFLT bwi;
        SKFLT fi;
        bw_Hz = (pow(2.0, bw/1200.0) - 1.0) * f * nh;
        bwi = bw_Hz/(2.0*N);
        fi = f*nh/N;
        for (i = 0; i < N/2 ; i++) {
            SKFLT hprofile;
            hprofile = profile((i / (SKFLT) N) - fi, bwi);
            freq_amp[i] += hprofile*A[nh];
        }
    }

    for (i = 0; i < N/2; i++) {
        freq_phase[i] = sk_core_randf(core) * 2.0 * M_PI;
    };

    apply_ifft(N,freq_amp,freq_phase,smp);
    normalize(N,smp);

    free(freq_amp);
    free(freq_phase);
}

int sk_padsynth(sk_core *core)
{
    sk_table *ps;
    sk_table *amps;
    SKFLT freq;
    SKFLT bw;
    int rc;

    rc = sk_param_get_constant(core, &bw);
    SK_ERROR_CHECK(rc);
    rc = sk_param_get_constant(core, &freq);
    SK_ERROR_CHECK(rc);
    rc = sk_core_table_pop(core, &amps);
    SK_ERROR_CHECK(rc);
    rc = sk_core_table_pop(core, &ps);
    SK_ERROR_CHECK(rc);

    sk_padsynth_dsp(core, ps, amps, freq, bw);

    rc = sk_core_table_push(core, ps);
    SK_ERROR_CHECK(rc);
    return 0;
}
