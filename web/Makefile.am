EXTRA_DIST = ipc *.html
CLEANFILES = *~ ipc/*~
webroot = @WEBROOT@

install-data-local:
	mkdir -p $(WEBROOT)/bibledit
	cp *.html $(WEBROOT)/bibledit
	cp -r ipc $(WEBROOT)/bibledit
	chmod 0777 $(WEBROOT)/bibledit/ipc/messages


