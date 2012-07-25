EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)

uninstall-local:
	rm -rf $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)

