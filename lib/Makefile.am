noinst_LIBRARIES = libbibledit.a

libbibledit_a_SOURCES = \
	library/bibledit.cpp \
	webserver/webserver.cpp \
	webserver/http.cpp \
	webserver/request.cpp \
	webserver/io.c \
	bootstrap/bootstrap.cpp \
	filter/url.cpp \
	filter/string.cpp \
	filter/roles.cpp \
	filter/md5.cpp \
	filter/usfm.cpp \
	filter/archive.cpp \
	filter/text.cpp \
	filter/passage.cpp \
	filter/customcss.cpp \
	filter/bibleworks.cpp \
	filter/git.cpp \
	filter/diff.cpp \
	filter/abbreviations.cpp \
	flate/flate.cpp \
	assets/view.cpp \
	assets/page.cpp \
	assets/header.cpp \
	assets/builder.cpp \
	index/index.cpp \
	config/globals.cpp \
	menu/logic.cpp \
	menu/main.cpp \
	menu/user.cpp \
	locale/translate.cpp \
	database/config/general.cpp \
	database/config/bible.cpp \
	database/config/user.cpp \
	database/users.cpp \
	database/logs.cpp \
	database/sqlite.cpp \
	database/styles.cpp \
	database/bibles.cpp \
	database/search.cpp \
	database/books.cpp \
	database/bibleactions.cpp \
	database/check.cpp \
	database/commits.cpp \
	database/confirm.cpp \
	database/history.cpp \
	database/ipc.cpp \
	database/jobs.cpp \
	database/kjv.cpp \
	database/morphhb.cpp \
	database/sblgnt.cpp \
	database/offlineresources.cpp \
	database/sprint.cpp \
	database/mail.cpp \
	database/navigation.cpp \
	database/resources.cpp \
	database/usfmresources.cpp \
	database/mappings.cpp \
	database/noteactions.cpp \
	database/versifications.cpp \
	database/modifications.cpp \
	database/notes.cpp \
	database/volatile.cpp \
	database/maintenance.cpp \
	session/logic.cpp \
	session/login.cpp \
	session/logout.cpp \
	session/password.cpp \
	session/signup.cpp \
	parsewebdata/ParseMultipartFormData.cpp \
	parsewebdata/ParseWebData.cpp \
	setup/index.cpp \
	journal/index.cpp \
	styles/logic.cpp \
	styles/indext.cpp \
	styles/indexm.cpp \
	styles/sheetm.cpp \
	styles/view.cpp \
	styles/css.cpp \
	styles/sheets.cpp \
	text/text.cpp \
	esword/text.cpp \
	onlinebible/text.cpp \
	html/text.cpp \
	odf/text.cpp \
	timer/index.cpp \
	tasks/logic.cpp \
	tasks/run.cpp \
	config/logic.cpp \
	bible/logic.cpp \
	bible/manage.cpp \
	bible/settings.cpp \
	bible/book.cpp \
	bible/chapter.cpp \
	bible/import_usfm.cpp \
	bible/import_bibleworks.cpp \
	bible/import_task.cpp \
	bible/abbreviations.cpp \
	bible/order.cpp \
	bible/css.cpp \
	notes/logic.cpp \
	trash/handler.cpp \
	sync/logic.cpp \
	help/index.cpp \
	confirm/worker.cpp \
	email/index.cpp \
	email/send.cpp \
	email/receive.cpp \
	user/notifications.cpp \
	user/account.cpp \
	manage/indexing.cpp \
	manage/users.cpp \
	administration/language.cpp \
	administration/timezone.cpp \
	search/rebibles.cpp \
	search/renotes.cpp \
	access/user.cpp \
	access/bible.cpp \
	dialog/entry.cpp \
	dialog/list.cpp \
	dialog/yes.cpp \
	dialog/color.cpp \
	dialog/books.cpp \
	dialog/upload.cpp \
	fonts/logic.cpp \
	fonts/index.cpp \
	versification/index.cpp \
	versification/system.cpp \
	book/create.cpp \
	compare/index.cpp \
	compare/compare.cpp \
	jobs/index.cpp \
	editverse/index.cpp \
	editverse/id.cpp \
	editverse/load.cpp \
	editverse/save.cpp \
	navigation/passage.cpp \
	navigation/update.cpp \
	navigation/poll.cpp \
	ipc/focus.cpp \
	checksum/logic.cpp \
	editusfm/focus.cpp \
	editusfm/id.cpp \
	editusfm/index.cpp \
	editusfm/load.cpp \
	editusfm/offset.cpp \
	editusfm/save.cpp \
	editor/styles.cpp \
	editor/export.cpp \
	editor/import.cpp \
	edit/edit.cpp \
	edit/focus.cpp \
	edit/id.cpp \
	edit/index.cpp \
	edit/load.cpp \
	edit/offset.cpp \
	edit/save.cpp \
	edit/styles.cpp \
	search/search.cpp \
	search/index.cpp \
	search/select.cpp \
	search/replace.cpp \
	search/getids.cpp \
	search/replacepre.cpp \
	search/replacego.cpp \
	search/search2.cpp \
	search/replace2.cpp \
	search/replacepre2.cpp \
	search/getids2.cpp \
	search/replacego2.cpp \
	search/similar.cpp \
	tmp/tmp.cpp

bin_PROGRAMS = server unittest

server_SOURCES = executable/bibledit.cpp

server_LDADD = libbibledit.a

unittest_SOURCES = \
	unittests/unittest.cpp \
	unittests/utilities.cpp \
	unittests/libraries.cpp \
	unittests/filters.cpp \
	unittests/databases.cpp

unittest_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking
AM_CXXFLAGS += `xml2-config --cflags`	
AM_CXXFLAGS += $(SQLITE_CFLAGS)
AM_CXXFLAGS += $(CURL_CFLAGS)

LIBS = -Wall -lpthread
LIBS += `xml2-config --libs`
LIBS += $(SQLITE_LIBS)
LIBS += $(CURL_LIBS)

EXTRA_DIST = a* bible bibledit bibles book bootstrap c* d* D* e* f* g* h* i* j* k* l* m* n* o* p* r* s* t* u* v* w* x*

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`

CLEANFILES = bibledit*.gz *~
