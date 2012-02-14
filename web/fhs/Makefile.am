EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)

uninstall-local:
	rm -rf $(DESTDIR)/$(packagelocalstatedir)

