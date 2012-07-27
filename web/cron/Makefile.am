EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	echo "* * * * * root cd $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/timer; nice -n 30 php index.php; cd" > $(DESTDIR)/etc/cron.d/bibledit-web-$(LOCATION)

uninstall-local:
	rm -f $(DESTDIR)/etc/cron.d/bibledit-web-$(LOCATION)

