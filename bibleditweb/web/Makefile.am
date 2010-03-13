EXTRA_DIST = *
CLEANFILES = *~
webroot = @WEBROOT@

install-data-local:
	rm -rf $(WEBROOT)/bibledit
	mkdir -p $(WEBROOT)/bibledit
	cp -r * $(WEBROOT)/bibledit
	chmod -R 0777 $(WEBROOT)/bibledit
	chmod -R 0777 $(WEBROOT)/bibledit/*

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
