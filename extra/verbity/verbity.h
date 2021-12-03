#ifndef SK_VERBITY_H
#define SK_VERBITY_H

#ifndef SKFLT
#define SKFLT float
#endif

typedef struct sk_verbity sk_verbity;

#ifdef SK_VERBITY_PRIV
struct sk_verbity {
    SKFLT bigness;
    SKFLT longness;
    SKFLT darkness;

	SKFLT iirAL;
	SKFLT iirBL;

	SKFLT aIL[6480];
	SKFLT aJL[3660];
	SKFLT aKL[1720];
	SKFLT aLL[680];

	SKFLT aAL[9700];
	SKFLT aBL[6000];
	SKFLT aCL[2320];
	SKFLT aDL[940];

	SKFLT aEL[15220];
	SKFLT aFL[8460];
	SKFLT aGL[4540];
	SKFLT aHL[3200];

	SKFLT feedbackAL;
	SKFLT feedbackBL;
	SKFLT feedbackCL;
	SKFLT feedbackDL;
	SKFLT previousAL;
	SKFLT previousBL;
	SKFLT previousCL;
	SKFLT previousDL;

	SKFLT lastRefL[7];
	SKFLT thunderL;

	SKFLT iirAR;
	SKFLT iirBR;

	SKFLT aIR[6480];
	SKFLT aJR[3660];
	SKFLT aKR[1720];
	SKFLT aLR[680];

	SKFLT aAR[9700];
	SKFLT aBR[6000];
	SKFLT aCR[2320];
	SKFLT aDR[940];

	SKFLT aER[15220];
	SKFLT aFR[8460];
	SKFLT aGR[4540];
	SKFLT aHR[3200];

	SKFLT feedbackAR;
	SKFLT feedbackBR;
	SKFLT feedbackCR;
	SKFLT feedbackDR;
	SKFLT previousAR;
	SKFLT previousBR;
	SKFLT previousCR;
	SKFLT previousDR;

	SKFLT lastRefR[7];
	SKFLT thunderR;

	int countA, delayA;
	int countB, delayB;
	int countC, delayC;
	int countD, delayD;
	int countE, delayE;
	int countF, delayF;
	int countG, delayG;
	int countH, delayH;
	int countI, delayI;
	int countJ, delayJ;
	int countK, delayK;
	int countL, delayL;
	int cycle;

    int sr;

    SKFLT psize;
    SKFLT onedsr;
};
#endif

void sk_verbity_init(sk_verbity *v, int sr);
void sk_verbity_tick(sk_verbity *v,
                     SKFLT *inL, SKFLT *inR,
                     SKFLT *outL, SKFLT *outR);
void sk_verbity_bigness(sk_verbity *c, SKFLT bigness);
void sk_verbity_longness(sk_verbity *c, SKFLT longness);
void sk_verbity_darkness(sk_verbity *c, SKFLT darkness);
#endif
