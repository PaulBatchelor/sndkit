TEX=tex
DVIPNG=dvipng

include figs.mk

default: $(FIGS)

%.png %2x.png: %.tex
	echo $*
	$(TEX) $<
	$(DVIPNG) -D150 -o $@ $*.dvi
	$(DVIPNG) -D300 -o $*2x.png $*.dvi
	$(RM) $*.dvi
	$(RM) $*.log

clean:
	$(RM) $(FIGS)
