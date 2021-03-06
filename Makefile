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
biramp.c biramp.h \
core.c core.h \
expmap.c expmap.h \
scale.c scale.h \

OBJ=\
bigverb.o \
bitnoise.o \
chaosnoise.o \
dcblocker.o \
fmpair.o \
modalres.o \
osc.o \
peakeq.o \
phasewarp.o \
rline.o \
valp1.o \
vardelay.o \
oscf.o \
bezier.o \
phasor.o \
swell.o \
biramp.o \
core.o \
patchwerk/patchwerk.o \
scale.o \

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

clean:
	$(RM) $(TANGLED)
	$(RM) worgle/worglite
	$(RM) libsndkit.a
	$(RM) $(OBJ)
