noinst_LIBRARIES = libtest.a

libtest_a_SOURCES = library.cpp

bin_PROGRAMS = bibledit

bibledit_SOURCES = test.cpp

bibledit_LDADD = libtest.a

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking
AM_CXXFLAGS += `xml2-config --cflags`	
AM_CXXFLAGS += $(SQLITE_CFLAGS)
AM_CXXFLAGS += $(CURL_CFLAGS)
AM_CXXFLAGS += $(GIT_CFLAGS)
AM_CXXFLAGS += $(OPENSSL_CFLAGS)

LIBS = -Wall -lpthread -L.
LIBS += `xml2-config --libs`
LIBS += $(SQLITE_LIBS)
LIBS += $(CURL_LIBS)
LIBS += $(GIT_LIBS)
LIBS += $(OPENSSL_LIBS)

CLEANFILES = test*.gz *~

applicationdir = $(datadir)/applications
application_DATA = bibledit.desktop

appicondir = $(datadir)/pixmaps
appicon_DATA = bibledit.xpm

