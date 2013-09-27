# Copyright (©) 2003-2013 Teus Benschop.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
  
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

EXTRA_DIST = *
CLEANFILES = *~

install-data-local:
	echo  > $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo Alias /$(LOCATION) /usr/share/bibledit-web/$(LOCATION) >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo "<Directory /usr/share/bibledit-web/$(LOCATION)>" >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo "	Options FollowSymLinks Indexes" >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo "	DirectoryIndex index.php" >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo "	ErrorDocument 404 /$(LOCATION)/index/index.php" >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	echo "</Directory>" >> $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	- service apache2 restart

uninstall-local:
	- service apache2 restart

# http://webapps-common.alioth.debian.org/draft/html/
# http://www.control-escape.com/web/configuring-apache2-debian.html
# The shell line (-) tells 'make' to ignore the exit status of the 
# shell command. This is useful during debuild
# in case apache2 cannot be restarted due to lack of privileges.
