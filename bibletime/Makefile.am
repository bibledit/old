man_MANS = *.1

appicondir = $(datadir)/pixmaps
appicon_DATA = bibledit-bibletime.xpm
dist_pkgdata_DATA = *.xpm *.xml
applicationdir = $(datadir)/applications
application_DATA = bibledit-bibletime.desktop

bin_PROGRAMS = bibledit-bibletime

bibledit_bibletime_SOURCES = bibledit-bibletime.cpp

AM_CXXFLAGS = -Wall 
AM_CXXFLAGS += $(GOBJECT_CFLAGS)
AM_CXXFLAGS += $(GTHREAD_CFLAGS)
AM_CXXFLAGS += $(GLIB_CFLAGS)
AM_CXXFLAGS += $(GTK_CFLAGS)
AM_CXXFLAGS += $(DBUSGLIB_CFLAGS)
AM_CXXFLAGS += $(LIBSOUP_CFLAGS)

LIBS = -Wall 
LIBS += $(GOBJECT_LIBS)
LIBS += $(GTHREAD_LIBS)
LIBS += $(GLIB_LIBS)
LIBS += $(GTK_LIBS)
LIBS += $(DBUSGLIB_LIBS)
LIBS += $(LIBSOUP_LIBS)

if LINUX
endif
if DARWIN
endif
if WIN32
LIBS += -mwindows -lws2_32
endif

EXTRA_DIST = *.h *.1 TODO

CLEANFILES = bibledit*.gz *~
