noinst_LIBRARIES = libbibledit.a

libbibledit_a_SOURCES = \
	library/bibledit.cpp \
	webserver/webserver.cpp \
	webserver/http.cpp \
	sqlite/sqlite3.c \
	bootstrap/bootstrap.cpp \
	filter/url.cpp

bin_PROGRAMS = bibledit

bibledit_SOURCES = executable/bibledit.cpp

bibledit_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall 

LIBS = -Wall 

EXTRA_DIST = a* bible bibledit bibles book bootstrap c* d* D* e* f* g* h* i* j* k* l* m* n* o* p* r* s* t* u* v* w* x*

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`

CLEANFILES = bibledit*.gz *~
