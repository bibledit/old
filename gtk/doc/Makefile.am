dist_pkgdata_DATA = *.html *.css
EXTRA_DIST = site
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)
	cp -r site $(DESTDIR)$(prefix)/$(packagedatadir)


