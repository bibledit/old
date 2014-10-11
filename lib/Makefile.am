noinst_LIBRARIES = libbibledit.a

libbibledit_a_SOURCES = \
	library/library.cc 
	library/library.h

bin_PROGRAMS = bibledit

bibledit_SOURCES = \
	executable/bibledit.cc \
	executable/bibledit.h

bibledit_LDADD = libbibledit.a

AM_CXXFLAGS = -Wall 
AM_CXXFLAGS += -Ilibrary

LIBS = -Wall 
LIBS += -I.
