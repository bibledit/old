SUBDIRS = man

appicondir = $(datadir)/pixmaps
appicon_DATA = bibledit-xiphos.xpm
dist_pkgdata_DATA = *.xpm *.xml
applicationdir = $(datadir)/applications
application_DATA = bibledit-xiphos.desktop

bin_PROGRAMS = bibledit-xiphos

bibledit_xiphos_SOURCES = bibledit-xiphos.cpp

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

EXTRA_DIST = *.h TODO *.desktop

CLEANFILES = bibledit*.gz *~
