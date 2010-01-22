EXTRA_DIST = ipc *.html
CLEANFILES = *~ ipc/*~
webroot = @WEBROOT@

install-data-local:
	mkdir -p $(WEBROOT)/bibledit
	cp -r * $(WEBROOT)/bibledit
	chmod 0777 $(WEBROOT)/bibledit/ipc/messages
	chmod 0777 $(WEBROOT)/bibledit/templates_c


