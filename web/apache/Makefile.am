EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)/$(systemconfigdir)
	cp apache.conf $(DESTDIR)/$(systemconfigdir)
	rm -f $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	ln -s $(DESTDIR)/$(systemconfigdir)/apache.conf $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	service apache2 restart

uninstall-local:
	rm -f $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	rm -rf $(DESTDIR)/$(systemconfigdir)
	service apache2 restart

# http://webapps-common.alioth.debian.org/draft/html/
# http://www.control-escape.com/web/configuring-apache2-debian.html
