if [ "$#" -lt 2 ]
then
    printf "Usage: $0 in.lil out.lil\n"
    exit 1
fi

IN=$1
OUT=$2
sed -e "/^wavout/d" -e "/^computes/d" < $IN > $OUT
echo "verify" >> $OUT
printf "#md5 verify %s\n" $(../sndkit $OUT) >> $OUT
sed -e "/^verify/d" -e "s/^#md5 //" -i $OUT
