noinst_LIBRARIES = liblibrary.a

liblibrary_a_SOURCES = library.cpp

bin_PROGRAMS = bibledit

bibledit_SOURCES = bibledit.cpp

bibledit_LDADD = liblibrary.a

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking
AM_CXXFLAGS += $(SQLITE_CFLAGS)
AM_CXXFLAGS += $(CURL_CFLAGS)
AM_CXXFLAGS += $(OPENSSL_CFLAGS)

LIBS = -Wall -lpthread -L.
LIBS += $(SQLITE_LIBS)
LIBS += $(CURL_LIBS)
LIBS += $(OPENSSL_LIBS)

CLEANFILES = *.gz *~
