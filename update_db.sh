if [ ! -f a.db ]
then
   echo "No db found. Generating."
   weewiki janet mkdb.janet
   exit
fi

if [ a.db -nt mkdb.janet ]
then
    echo "Nothing needed to be done."
else
	cmp mkdb.janet mkdb.janet.old ||\
		weewiki janet mkdb.janet
	cp mkdb.janet mkdb.janet.old

fi
