dist_pkgdata_DATA = *.html *.css *.pl *.xml
EXTRA_DIST = site
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)
	cp -r site $(DESTDIR)$(prefix)/$(packagedatadir)


