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
	cp createdb.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp version.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp logs.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp versions.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp users.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp config.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp mail.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp mailer.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp confirm.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp books.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp versification.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp styles.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp bible.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp snapshots.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp ipc.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp sessions.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp notes.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp shell.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp git.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp check.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp logger.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp changes.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp resources.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	php resources.php ../web/resource > /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp usfmresources.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp menu.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp offlineresources.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp consistency.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql
	cp changes_user.sql /tmp/bibledit.sql
	sed -i "s/BIBLEDITDATABASE/$(LOCATION)/g" /tmp/bibledit.sql
	mysql -uroot -p$(MYSQL_ROOT_PASSWORD) < /tmp/bibledit.sql
	rm /tmp/bibledit.sql

