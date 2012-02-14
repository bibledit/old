EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/changes
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/exports
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)
	cp -r * $(DESTDIR)$(prefix)/$(packagedatadir)
	chown -R www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/changes
	- ln -s /$(packagelocalstatedir)/changes $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/changes
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/changes
	chown www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/changes
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/exports
	- ln -s /$(packagelocalstatedir)/exports $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/exports
	chown www-data:www-data $(DESTDIR)/$(packagelocalstatedir)/exports
	chown www-data:www-data $(DESTDIR)$(prefix)/$(packagedatadir)/downloads/exports

clean-local:
	find . -iname *~ -delete

