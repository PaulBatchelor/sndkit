.PHONY: tangle

PORT?=8080
WIKI_PATH=sndkit
WORGLE=$(abspath worgle/worglite)
WORGLE_FLAGS=-g -Werror

TANGLED=\
bigverb.c bigverb.h \
bitnoise.c bitnoise.h \
chaosnoise.c chaosnoise.h \
dcblocker.c dcblocker.h \
fmpair.c fmpair.h \
modalres.c modalres.h \
osc.c osc.h \
peakeq.c peakeq.h \
phasewarp.c phasewarp.h \
rline.c rline.h \
valp1.c valp1.h \
vardelay.c vardelay.h \
oscf.c oscf.h \
bezier.c bezier.h \
phasor.c phasor.h \
swell.c swell.h \

.SUFFIX: .org .c

.SUFFIXES: .org .c
.org.c: .org .c
	@echo "WORGLE $<"
	@cd $(dir $<); $(WORGLE) $(WORGLE_FLAGS) $(notdir $<)

default: update

sync:
	weewiki sync

update:
	$(MAKE) db
	$(MAKE) keys.db
	$(MAKE) sync
	$(MAKE) dump

export: db
	$(RM) -r _site/$(WIKI_PATH)
	mkdir -p _site/$(WIKI_PATH)
	weewiki sync
	weewiki export
	mkdir -p _site/$(WIKI_PATH)/_fig
	rsync -rvt _fig/*.png _site/$(WIKI_PATH)/_fig
	mv mkdb.janet mkdb.janet.old
	weewiki dump mkdb.janet

db:
	@sh update_db.sh

dump:
	weewiki dump mkdb.janet

transfer:
	$(RM) -r _live/$(WIKI_PATH)
	mkdir -p _live/$(WIKI_PATH)
	rsync -rvt _site/$(WIKI_PATH)/* _live/$(WIKI_PATH)
	rsync -rvt _img _live/$(WIKI_PATH)/


worgle/worglite: worgle/worgle.c worgle/parg.c
	$(CC) -std=c89 -Wall -pedantic -O3 -DWORGLITE $^ -o $@

server:
	weewiki server - $(PORT)

keys:
	$(RM) keys.txt
	$(MAKE) keys.txt

keys.txt:
	weewiki keyscrape > $@

keys.db: keys.txt
	./keys2db < keys.txt | sqlite3 $@

tangle: worgle/worglite $(TANGLED)

clean:
	$(RM) $(TANGLED)
	$(RM) worgle/worglite
