noinst_LIBRARIES = libbibledit.a

libbibledit_a_SOURCES = \
	library/bibledit.cpp \
	webserver/webserver.cpp \
	webserver/http.cpp \
	webserver/request.cpp \
	bootstrap/bootstrap.cpp \
	filter/url.cpp \
	filter/string.cpp \
	filter/roles.cpp \
	filter/md5.cpp \
	filter/usfm.cpp \
	filter/archive.cpp \
	filter/text.cpp \
	filter/passage.cpp \
	flate/flate2.cpp \
	assets/view.cpp \
	assets/page.cpp \
	assets/header.cpp \
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
	session/logic.cpp \
	session/login.cpp \
	session/logout.cpp \
	parsewebdata/ParseMultipartFormData.cpp \
	parsewebdata/ParseWebData.cpp \
	setup/index.cpp \
	journal/index.cpp \
	styles/logic.cpp \
	text/text.cpp \
	esword/text.cpp \
	onlinebible/text.cpp \
	html/text.cpp \
	odf/text.cpp \
	timer/index.cpp \
	tasks/logic.cpp \
	tasks/run.cpp

bin_PROGRAMS = server unittest

server_SOURCES = executable/bibledit.cpp

server_LDADD = libbibledit.a

unittest_SOURCES = \
	unittests/unittest.cpp \
	unittests/utilities.cpp \
	unittests/tests1.cpp \
	unittests/database_config.cpp \
	unittests/filters.cpp \
	unittests/databases.cpp

unittest_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11
AM_CXXFLAGS += `xml2-config --cflags`
AM_CXXFLAGS += $(SQLITE_CFLAGS)

LIBS = -Wall -lpthread
LIBS += `xml2-config --libs`
LIBS += $(SQLITE_LIBS)

EXTRA_DIST = a* bible bibledit bibles book bootstrap c* d* D* e* f* g* h* i* j* k* l* m* n* o* p* r* s* t* u* v* w* x*

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`

CLEANFILES = bibledit*.gz *~
