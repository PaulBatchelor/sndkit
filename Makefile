.PHONY: tangle

PORT?=8080
WIKI_PATH=sndkit
WORGLE=$(abspath worgle/worglite)
WORGLE_FLAGS=-g -Werror

C89?=$(CC) -std=c89
C99?=$(CC) -std=c99

CFLAGS += -Ipatchwerk -I. -Inodes
CFLAGS += -Wall
CFLAGS += -O3
CFLAGS += -g

TANGLED=\
dsp/bigverb.c dsp/bigverb.h \
dsp/bitnoise.c dsp/bitnoise.h \
dsp/chaosnoise.c dsp/chaosnoise.h \
dsp/dcblocker.c dsp/dcblocker.h \
dsp/fmpair.c dsp/fmpair.h \
dsp/modalres.c dsp/modalres.h \
dsp/osc.c dsp/osc.h \
dsp/peakeq.c dsp/peakeq.h \
dsp/phasewarp.c dsp/phasewarp.h \
dsp/rline.c dsp/rline.h \
dsp/valp1.c dsp/valp1.h \
dsp/vardelay.c dsp/vardelay.h \
dsp/oscf.c dsp/oscf.h \
dsp/bezier.c dsp/bezier.h \
dsp/phasor.c dsp/phasor.h \
dsp/swell.c dsp/swell.h \
dsp/biramp.c dsp/biramp.h \
dsp/expmap.c dsp/expmap.h \
dsp/scale.c dsp/scale.h \
dsp/rephasor.c dsp/rephasor.h \
dsp/gen.c dsp/gen.h \
dsp/smoother.c dsp/smoother.h \
dsp/metro.c dsp/metro.h \
dsp/expon.c dsp/expon.h \
dsp/chorus.c dsp/chorus.h \
dsp/bitosc.c dsp/bitosc.h \

TANGLED+=core.c core.h

OBJ=\
dsp/bigverb.o \
dsp/bitnoise.o \
dsp/chaosnoise.o \
dsp/dcblocker.o \
dsp/fmpair.o \
dsp/modalres.o \
dsp/osc.o \
dsp/peakeq.o \
dsp/phasewarp.o \
dsp/rline.o \
dsp/valp1.o \
dsp/vardelay.o \
dsp/oscf.o \
dsp/bezier.o \
dsp/phasor.o \
dsp/swell.o \
dsp/biramp.o \
core.o \
patchwerk/patchwerk.o \
dsp/scale.o \
dsp/rephasor.o \
dsp/gen.o \
dsp/smoother.o \
dsp/metro.o \
dsp/mtof.o \
dsp/expon.o \
dsp/expmap.o \
dsp/chorus.o \
dsp/bitosc.o \

# headers to install
HEADERS=\
	lil/lil.h \
	nodes/sklil.h \
	nodes/sknodes.h \
	patchwerk/patchwerk.h \
	core.h

include nodes/config.mk

OBJ+=lil/lil.c99
OBJ+=lil/lil_main.c99
OBJ+=nodes/loader.o
OBJ+=nodes/sklil.o

.SUFFIX: .org .c

.SUFFIXES: .org .c
.org.c:
	@echo "WORGLE $<"
	@cd $(dir $<); $(WORGLE) $(WORGLE_FLAGS) $(notdir $<)

.SUFFIXES: .c .o
.c.o:
	@echo "$(C89) $<"
	@$(C89) -pedantic -c $(CFLAGS) $< -o $@

.SUFFIXES: .c .c99
.c.c99:
	@echo "$(C99) $<"
	@$(C99) -c $(CFLAGS) $< -o $@

default: tangle sndkit libsndkit.a

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

libsndkit.a: $(OBJ)
	@echo "Building $@"
	@$(AR) rcs $@ $(OBJ)

sndkit: main.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

install: libsndkit.a sndkit
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/bin
	mkdir -p /usr/local/include/sndkit
	cp libsndkit.a /usr/local/lib
	cp $(HEADERS) /usr/local/include/sndkit
	cp sndkit /usr/local/bin

clean:
	$(RM) $(TANGLED)
	$(RM) worgle/worglite
	$(RM) libsndkit.a
	$(RM) $(OBJ)
