bin_PROGRAMS = bibledit-help

bibledit_help_SOURCES = bibledit-help.cpp

AM_CXXFLAGS = -Wall 
AM_CXXFLAGS += $(GLIB_CFLAGS)
AM_CXXFLAGS += $(GTK_CFLAGS)
AM_CXXFLAGS += $(GTKHTML3_CFLAGS)
AM_CXXFLAGS += $(LIBCURL_CPPFLAGS)

LIBS = -Wall 
LIBS += $(GLIB_LIBS)
LIBS += $(GTK_LIBS)
LIBS += $(GTKHTML3_LIBS)
LIBS += $(LIBCURL)

if LINUX
endif
if DARWIN
endif
if WIN32
LIBS += -mwindows
endif

CLEANFILES = *~
