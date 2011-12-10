EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	echo "* * * * * root cd $(DESTDIR)$(prefix)/$(packagedatadir)/www/timer; nice -n 30 php index.php; cd"  > bibledit-web
	ps ax > processes
	if [ `grep -c debuild processes` -eq 0 ]; then cp bibledit-web /etc/cron.d/ ; fi
	rm -f bibledit-web
	rm -f processes

uninstall-local:
	rm -f /etc/cron.d/bibledit-web

# The installer checks whether debuild runs.
# If it runs, it won't install anything into cron.d
# This prevents the host system from being affected
# in case of building the package through debuild.
