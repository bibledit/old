noinst_LIBRARIES = libbibledit.a

libbibledit_a_SOURCES = \
	library/bibledit.cpp \
	webserver/webserver.cpp \
	webserver/http.cpp \
	sqlite/sqlite3.c

bin_PROGRAMS = bibledit

bibledit_SOURCES = executable/bibledit.cpp

bibledit_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall 

LIBS = -Wall 

EXTRA_DIST = D* e* l* w* s*

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`

CLEANFILES = bibledit*.gz *~
