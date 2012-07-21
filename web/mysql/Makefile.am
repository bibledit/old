EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	cp setup.sql /tmp/setup.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/setup.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/setup.sql
	rm /tmp/setup.sql

uninstall-local:
	rm -f $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	rm -rf $(DESTDIR)/$(systemconfigdir)
	service apache2 restart

