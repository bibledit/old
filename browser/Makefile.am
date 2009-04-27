bin_PROGRAMS = bibledit-help

bibledit_help_SOURCES = bibledit-help.cpp

AM_CXXFLAGS = -Wall 
AM_CXXFLAGS += $(GLIB_CFLAGS)
AM_CXXFLAGS += $(GTK_CFLAGS)
AM_CXXFLAGS += $(WEBKIT_CFLAGS)

LIBS = -Wall 
LIBS += $(GLIB_LIBS)
LIBS += $(GTK_LIBS)
LIBS += $(WEBKIT_LIBS)

if LINUX
endif
if DARWIN
endif
if WIN32
LIBS += -mwindows -lws2_32
endif

CLEANFILES = *~

EXTRA_DIST = *.h
