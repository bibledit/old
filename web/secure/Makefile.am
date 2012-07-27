EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	chown -R www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	chown -R www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	chown -R www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/changes
	chown -R www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	chown -R www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/exports
	chown -R www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports

