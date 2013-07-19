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

