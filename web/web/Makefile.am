EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	cp -r * $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	chown -R www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/changes
	- ln -s /$(packagelocalstatedir)/$(LOCATION)/changes $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/changes
	chown www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/exports
	- ln -s /$(packagelocalstatedir)/exports $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/exports
	chown www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports

clean-local:
	find . -iname *~ -delete

