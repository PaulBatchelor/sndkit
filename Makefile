.PHONY: tangle

PORT?=8080
WIKI_PATH=sndkit
WORGLE=$(abspath worgle/worglite)
WORGLE_FLAGS=-g -Werror

OUTDIR=out

C89?=$(CC) -std=c89
C99?=$(CC) -std=c99

CFLAGS += -Igraforge -I. -Inodes
CFLAGS += -Wall
CFLAGS += -O3
CFLAGS += -g

LDFLAGS += -lm

ALGOS=\
	bitnoise \
	bigverb \
	chaosnoise \
	dcblocker \
	fmpair \
	modalres \
	osc \
	peakeq \
	phasewarp \
	rline \
	valp1 \
	vardelay \
	oscf \
	bezier \
	phasor \
	swell \
	biramp \
	expmap \
	scale \
	rephasor \
	gen \
	smoother \
	metro \
	mtof \
	expon \
	chorus \
	bitosc \
	env \
	glottis \
	tract \
	blep \
	phsclk \
	vowel \
	vowshape \
	clkphs \
	butterworth \
	noise \
	sparse \
	softclip \
	crossfade \
	tseq \
	thresh \
	adsr \
	tenv \
	tgate \
	dblin \
	trand \
	tsmp \
	qgliss \
	tdiv \
	shelf \

# GNU Make is very convenient here...

TANGLED=$(addprefix dsp/, $(addsuffix .c, $(ALGOS)))
ALGO_HEADERS=$(addprefix dsp/, $(addsuffix .h, $(ALGOS)))
TANGLED+=core.c core.h

OBJ+=$(addprefix dsp/, $(addsuffix .o, $(ALGOS)))
OBJ+=core.o graforge/graforge.o

# headers to install
HEADERS=\
	lil/lil.h \
	nodes/sklil.h \
	nodes/sknodes.h \
	graforge/graforge.h \
	core.h

# lil source files
SRC+=lil/lil.c lil/lil.h lil/lil_main.c

# graforge source files
SRC+=graforge/graforge.c graforge/graforge.h

include nodes/config.mk
include extra/config.mk

OBJ+=lil/lil.o
OBJ+=lil/lil_main.o
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
	@$(C89) -c $(CFLAGS) -pedantic $< -o $@

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
	@echo "Building $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

install: libsndkit.a sndkit $(WORGLE)
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/bin
	mkdir -p /usr/local/include/sndkit
	mkdir -p /usr/local/include/sndkit/dsp
	mkdir -p /usr/local/include/sndkit/lil
	mkdir -p /usr/local/include/sndkit/nodes
	mkdir -p /usr/local/include/sndkit/graforge
	cp libsndkit.a /usr/local/lib
	cp $(ALGO_HEADERS) /usr/local/include/sndkit/dsp
	cp sndkit /usr/local/bin
	cp $(WORGLE) /usr/local/bin/skorgle
	cp nodes/dr_wav.h /usr/local/include/sndkit/nodes
	cp nodes/sklil.h /usr/local/include/sndkit/nodes
	cp nodes/sknodes.h /usr/local/include/sndkit/nodes
	cp graforge/graforge.h /usr/local/include/sndkit/graforge
	cp lil/lil.h /usr/local/include/sndkit/lil
	cp core.h /usr/local/include/sndkit/

copy: $(TANGLED) $(ALGO_HEADERS)
	@mkdir -p $(OUTDIR)
	@echo "Copying files to $(OUTDIR)"
	@rsync -Rrvt $(TANGLED) $(ALGO_HEADERS) $(SRC) $(OUTDIR)

clean:
	@echo "Cleaning"
	@$(RM) $(TANGLED)
	@$(RM) worgle/worglite
	@$(RM) libsndkit.a
	@$(RM) $(OBJ)
