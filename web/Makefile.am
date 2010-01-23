EXTRA_DIST = *
CLEANFILES = *~
webroot = @WEBROOT@

install-data-local:
	mkdir -p $(WEBROOT)/bibledit
	cp -r * $(WEBROOT)/bibledit
	chmod 0777 $(WEBROOT)/bibledit/ipc/messages
	chmod 0777 $(WEBROOT)/bibledit/templates_c
	chmod 0777 $(WEBROOT)/bibledit/cache

clean-local:
	find . -iname *~ -delete
