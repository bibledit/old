bin_PROGRAMS = bibledit-one

bibledit_one_SOURCES = bibledit-one.cpp

AM_CXXFLAGS = -Wall 
AM_CXXFLAGS += $(GLIB_CFLAGS)
AM_CXXFLAGS += $(GTK_CFLAGS)

LIBS = -Wall 
LIBS += $(GLIB_LIBS)
LIBS += $(GTK_LIBS)

if LINUX
endif
if DARWIN
endif
if WIN32
LIBS += -mwindows
endif

CLEANFILES = *~

EXTRA_DIST = *.h
