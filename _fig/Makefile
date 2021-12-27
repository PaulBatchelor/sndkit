TEX=tex
DVIPNG=dvipng

include figs.mk

default: $(FIGS)

%.png %2x.png: %.tex
	@echo "Generating $@"
	@$(TEX) $< 2>&1 > /dev/null
	@$(DVIPNG) -D150 -o $@ $*.dvi > /dev/null
	@$(DVIPNG) -D300 -o $*2x.png $*.dvi > /dev/null
	@$(RM) $*.dvi
	@$(RM) $*.log

clean:
	$(RM) $(FIGS)
