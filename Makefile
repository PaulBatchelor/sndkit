PORT?=8080
WIKI_PATH=sndkit

default: update

sync:
	weewiki sync

update:
	$(MAKE) db
	$(MAKE) sync
	$(MAKE) dump

export: db
	$(RM) -r _site/$(WIKI_PATH)
	mkdir -p _site/$(WIKI_PATH)
	weewiki sync
	weewiki export
	mv mkdb.janet mkdb.janet.old
	weewiki dump mkdb.janet

db:
	@sh update_db.sh

dump:
	weewiki dump mkdb.janet

transfer:
	mkdir -p _live/$(WIKI_PATH)
	cp -r _site/$(WIKI_PATH)/* _live/$(WIKI_PATH)

server:
	weewiki server - $(PORT)
