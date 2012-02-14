EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	echo "* * * * * root cd $(DESTDIR)$(prefix)/$(packagedatadir)/www/timer; nice -n 30 php index.php; cd"  > bibledit-web
	mkdir -p $(DESTDIR)/etc/cron.d/
	cp bibledit-web $(DESTDIR)/etc/cron.d/bibledit-web
	rm -f bibledit-web

uninstall-local:
	rm -f $(DESTDIR)/etc/cron.d/bibledit-web

