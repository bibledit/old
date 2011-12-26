EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	mkdir -p $(DESTDIR)/$(systemconfigdir)
	cp bibledit-web.conf $(DESTDIR)/$(systemconfigdir)
	mkdir -p $(DESTDIR)/etc/apache2/conf.d
	rm -f $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	- ln -s /$(systemconfigdir)/bibledit-web.conf $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	- service apache2 restart

uninstall-local:
	rm -f $(DESTDIR)/etc/apache2/conf.d/bibledit-web.conf
	rm -rf $(DESTDIR)/$(systemconfigdir)
	service apache2 restart

# http://webapps-common.alioth.debian.org/draft/html/
# http://www.control-escape.com/web/configuring-apache2-debian.html
# The shell line (-) tells 'make' to ignore the exit status of the 
# shell command. This is useful during debuild
# in case apache2 cannot be restarted due to lack of privileges.
