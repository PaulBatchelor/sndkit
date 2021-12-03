</$objtype/mkfile

TARG=sndkit
PROG=${TARG:%=$O.%}
BIN=/$objtype/bin/audio
CFLAGS=$CFLAGS -p -I. -Inodes -Igraforge -I/sys/include/npe -D__plan9__ -Dwchar_t=char -DDRWAV_HAS_WFOPEN -D_wfopen=fopen
WORGLE=`{pwd}/worgle/worgle.$cputype

HFILES=\
	lil/lil.h\

ORGFILES=\
	core.c\
	dsp/bezier.c\
	dsp/bigverb.c\
	dsp/biramp.c\
	dsp/bitnoise.c\
	dsp/bitosc.c\
	dsp/chaosnoise.c\
	dsp/chorus.c\
	dsp/dcblocker.c\
	dsp/env.c\
	dsp/expmap.c\
	dsp/expon.c\
	dsp/fmpair.c\
	dsp/gen.c\
	dsp/metro.c\
	dsp/modalres.c\
	dsp/mtof.c\
	dsp/osc.c\
	dsp/oscf.c\
	dsp/peakeq.c\
	dsp/phasewarp.c\
	dsp/phasor.c\
	dsp/phsclk.c\
	dsp/rephasor.c\
	dsp/rline.c\
	dsp/scale.c\
	dsp/smoother.c\
	dsp/swell.c\
	dsp/valp1.c\
	dsp/vardelay.c\

OFILES=\
	${ORGFILES:%.c=%.$O}\
	main.$O\
	lil/lil.$O\
	lil/lil_main.$O\
	nodes/loader.$O\
	nodes/sklil.$O\
	nodes/arith/arith.$O\
	nodes/arith/l_arith.$O\
	nodes/bezier/bezier.$O\
	nodes/bezier/l_bezier.$O\
	nodes/bigverb/bigverb.$O\
	nodes/bigverb/l_bigverb.$O\
	nodes/biramp/biramp.$O\
	nodes/biramp/l_biramp.$O\
	nodes/bitnoise/bitnoise.$O\
	nodes/bitnoise/l_bitnoise.$O\
	nodes/bitosc/bitosc.$O\
	nodes/bitosc/l_bitosc.$O\
	nodes/buffer/buffer.$O\
	nodes/chaosnoise/chaosnoise.$O\
	nodes/chaosnoise/l_chaosnoise.$O\
	nodes/chorus/chorus.$O\
	nodes/chorus/l_chorus.$O\
	nodes/dcblocker/dcblocker.$O\
	nodes/dcblocker/l_dcblocker.$O\
	nodes/env/env.$O\
	nodes/env/l_env.$O\
	nodes/expmap/expmap.$O\
	nodes/expmap/l_expmap.$O\
	nodes/expon/expon.$O\
	nodes/expon/l_expon.$O\
	nodes/fmpair/fmpair.$O\
	nodes/fmpair/l_fmpair.$O\
	nodes/gensine/gensine.$O\
	nodes/gensine/l_gensine.$O\
	nodes/metro/l_metro.$O\
	nodes/metro/metro.$O\
	nodes/modalres/l_modalres.$O\
	nodes/modalres/modalres.$O\
	nodes/mtof/l_mtof.$O\
	nodes/mtof/mtof.$O\
	nodes/osc/l_osc.$O\
	nodes/osc/osc.$O\
	nodes/oscf/l_oscf.$O\
	nodes/oscf/oscf.$O\
	nodes/peakeq/l_peakeq.$O\
	nodes/peakeq/peakeq.$O\
	nodes/phasewarp/l_phasewarp.$O\
	nodes/phasewarp/phasewarp.$O\
	nodes/phasor/l_phasor.$O\
	nodes/phasor/phasor.$O\
	nodes/reg/reg.$O\
	nodes/rephasor/l_rephasor.$O\
	nodes/rephasor/rephasor.$O\
	nodes/rline/l_rline.$O\
	nodes/rline/rline.$O\
	nodes/scale/l_scale.$O\
	nodes/scale/scale.$O\
	nodes/sine/l_sine.$O\
	nodes/sine/sine.$O\
	nodes/smoother/l_smoother.$O\
	nodes/smoother/smoother.$O\
	nodes/tabnew/tabnew.$O\
	nodes/valp1/l_valp1.$O\
	nodes/valp1/valp1.$O\
	nodes/vardelay/l_vardelay.$O\
	nodes/vardelay/vardelay.$O\
	nodes/wav/l_wavin.$O\
	nodes/wav/l_wavout.$O\
	nodes/wav/wavin.$O\
	nodes/wav/wavout.$O\
	graforge/graforge.$O\

CLEANFILES=$OFILES $PROG $ORGFILES ${ORGFILES:%.c=%.h}

default:V: all

all:V: $PROG

install:V:
	for (i in $TARG)
		mk $MKFLAGS $i.install

installall:V:
	for(objtype in $CPUS)
		mk $MKFLAGS install

installall:V:
	for(objtype in $CPUS)
		mk install

allall:V:
	for(objtype in $CPUS)
		mk all

nuke:V: clean
	rm -f *.acid

clean:V:
	rm -f *.[$OS] [$OS].out y.tab.? lex.yy.c y.debug y.output $CLEANFILES
	cd worgle
	mk clean
	cd ..

%.install:V: $BIN/%

$BIN/%: $O.%
	cp $prereq $BIN/$stem

$O.sndkit: $OFILES $LIB
	$LD $LDFLAGS -o $target $prereq

([^/]*)/(.*)\.$O:R: core.h \1/\2\.c
	$CC $CFLAGS -o $target $stem1/$stem2.c

([^/]*)\.$O:R: \1\.c
	$CC $CFLAGS -o $target $stem1.c

worgle:V: $WORGLE

$WORGLE:
	cd worgle
	@{objtype=$cputype mk}
	cd ..

core.$O: core.c core.h

dsp/%.$O: dsp/%.c dsp/%.h

core.c core.h:Q: $WORGLE
	$WORGLE -g -Werror core.org

dsp/%.c dsp/%.h:Q: dsp/%.org $WORGLE
	cd dsp
	$WORGLE -g -Werror $stem.org
	cd ..
