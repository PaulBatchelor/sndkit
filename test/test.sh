runtest () {
    ../sndkit t/$1.lil

    if [[ ! "$?" -eq 0 ]]
    then
        printf "fail"
        return
    fi
    printf "ok"
}

check () {
    printf "%s:\t%s\n" $1 $(runtest $1)
}

check zero
check sine
check noise
check tgate
check tract
