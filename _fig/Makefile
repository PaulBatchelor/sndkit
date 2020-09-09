TEX=tex
DVIPNG=dvipng

include figs.mk

default: $(FIGS)

%.png: %.tex
	echo $*
	$(TEX) $<
	$(DVIPNG) -D150 -o $@ $*.dvi
	$(RM) $*.dvi
	$(RM) $*.log

clean:
	$(RM) $(FIGS)
