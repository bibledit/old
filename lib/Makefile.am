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
	flate/flate.c \
	assets/view.cpp \
	assets/page.cpp \
	assets/header.cpp \
	index/index.cpp \
	config/globals.cpp \
	menu/logic.cpp \
	menu/main.cpp \
	locale/translate.cpp \
	database/config/general.cpp \
	database/config/bible.cpp \
	database/config/user.cpp \
	database/users.cpp \
	database/logs.cpp \
	database/sqlite.cpp \
	session/logic.cpp \
	session/login.cpp \
	parsewebdata/ParseMultipartFormData.cpp \
	parsewebdata/ParseWebData.cpp

bin_PROGRAMS = bibledit unittest

bibledit_SOURCES = executable/bibledit.cpp

bibledit_LDADD = libbibledit.a

unittest_SOURCES = unittests/unittest.cpp

unittest_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++0x
AM_CXXFLAGS += `xml2-config --cflags`
AM_CXXFLAGS += $(SQLITE_CFLAGS)

LIBS = -Wall 
LIBS += `xml2-config --libs`
LIBS += $(SQLITE_LIBS)

EXTRA_DIST = a* bible bibledit bibles book bootstrap c* d* D* e* f* g* h* i* j* k* l* m* n* o* p* r* s* t* u* v* w* x*

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`

CLEANFILES = bibledit*.gz *~
