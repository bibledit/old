EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	crontab -l | sed '/bibledit-web/d' > cron.txt
	echo "* * * * * cd $(DESTDIR)$(prefix)/$(packagedatadir)/www/timer; nice -n 30 php index.php; cd"  >> cron.txt
	crontab cron.txt
	rm -f cron.txt

uninstall-local:
	crontab -l | sed '/bibledit-web/d' | crontab

