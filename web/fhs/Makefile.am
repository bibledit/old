EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)/etc/apache2/conf.d
	mkdir -p $(DESTDIR)/etc/cron.d/
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/changes
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/exports
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	- ln -s /$(packagelocalstatedir)/$(LOCATION)/changes $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	- ln -s /$(packagelocalstatedir)/$(LOCATION)/exports $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## - ln -s /$(JQUERYPATH) $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui
	## - ln -s /$(JQUERYUIPATH) $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui

uninstall-local:
	rm -rf $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	- rm -f $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui

