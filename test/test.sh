runtest () {
    ../sndkit t/$1.lil

    if [ ! "$?" -eq 0 ]
    then
        printf "fail"
        return
    fi
    printf "ok"
}

check () {
    NSPACES=$(expr 16 - ${#1})
    printf "%s:%"$NSPACES"s\n" $1 $(runtest $1)
}

check zero
check sine
check noise
check tgate
check vowel
check adsr
check bezier
check bigverb
check biramp
check bitnoise
check bitosc
check blep
check chaosnoise
check chorus
check clkphs
check env
check expmap
check expon
check fmpair
check glottis
check metro
check modalres
check mtof
check noise
check oscf
check peakeq
check phasewarp
check phsclk
check rephasor
check rline
check softclip
check sparse
check tenv
check thresh
check tick
check tract
check tseq
check valp1
check vardelay
check osc
check phasor
check scale
check smoother
check verbity
check dblin
check tabdup
check trand
check crossfade
check qgliss
check tdiv
check highshelf
check lowshelf
check jitseg
check brown
