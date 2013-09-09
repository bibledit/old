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
	mkdir -p $(DESTDIR)/etc/apache2/conf.d
	mkdir -p $(DESTDIR)/etc/cron.d/
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)
	mkdir -p $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/changes
	mkdir -p $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)/exports
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	- ln -s /$(packagelocalstatedir)/$(LOCATION)/changes $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/changes
	- ln -s /$(packagelocalstatedir)/$(LOCATION)/exports $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/downloads/exports
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## - ln -s /$(JQUERYPATH) $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui
	## - ln -s /$(JQUERYUIPATH) $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui
	mkdir -p $(DESTDIR)/etc/php5/conf.d
	echo upload_max_filesize = 8M > $(DESTDIR)/etc/php5/conf.d/bibledit.ini

uninstall-local:
	rm -rf $(DESTDIR)/$(packagelocalstatedir)/$(LOCATION)
	- rm -f $(DESTDIR)/etc/apache2/conf.d/$(LOCATION).conf
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery
	## rm -rf $(DESTDIR)$(prefix)/$(packagedatadir)/$(LOCATION)/jquery-ui
	- rm -f $(DESTDIR)/etc/php5/conf.d/bibledit.ini

