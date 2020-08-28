PORT?=8080

default: update

sync:
	weewiki sync

update:
	$(MAKE) db
	$(MAKE) sync
	$(MAKE) dump

export: db
	$(RM) -r _site/wiki
	mkdir -p _site/wiki
	weewiki sync
	weewiki export
	mv mkdb.janet mkdb.janet.old
	weewiki dump mkdb.janet

db:
	@sh update_db.sh

dump:
	weewiki dump mkdb.janet

transfer:
	mkdir -p _live/wiki
	cp -r _site/wiki/* _live/wiki

server:
	weewiki server - $(PORT)
