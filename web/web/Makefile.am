EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/www/downloads/changes
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/www/downloads/exports
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/www
	cp -r * $(DESTDIR)$(prefix)/$(packagedatadir)/www
	chmod -R 0777 $(DESTDIR)$(prefix)/$(packagedatadir)/www
	chmod -R 0777 $(DESTDIR)$(prefix)/$(packagedatadir)/www/*
	find $(DESTDIR)$(prefix)/$(packagedatadir)/www -type f -print0 | xargs -0 chmod 0666
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/changes
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/exports
	ln -s $(DESTDIR)/$(packagelocalstatedir)/changes $(DESTDIR)$(prefix)/$(packagedatadir)/www/downloads/changes
	ln -s $(DESTDIR)/$(packagelocalstatedir)/exports $(DESTDIR)$(prefix)/$(packagedatadir)/www/downloads/exports

clean-local:
	find . -iname *~ -delete

#	Permission Level
# 0 None (the equivalent of a dash)
# 1 Executable, but cannot read or write
# 2 Write only, cannot read or execute
# 3 Write and Execute
# 4 Read-only
# 5 Read-only and Execute
# 6 Read, Write, but cannot execute
# 7	Read, Write and Execute
