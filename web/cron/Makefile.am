EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	echo "* * * * * root cd $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/timer; nice -n 30 php index.php; cd"  > bibledit-web-$(LOCATION)
	mkdir -p $(DESTDIR)/etc/cron.d/
	cp bibledit-web-$(LOCATION) $(DESTDIR)/etc/cron.d/bibledit-web-$(LOCATION)
	rm -f bibledit-web-$(LOCATION)

uninstall-local:
	rm -f $(DESTDIR)/etc/cron.d/bibledit-web-$(LOCATION)

