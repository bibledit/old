bin_PROGRAMS = bibledit-vacuum
bibledit_vacuum_SOURCES = bibledit-vacuum.cpp
CLEANFILES = *~
AM_CXXFLAGS = -Wall \
              `pkg-config sqlite3 --cflags` \
              `pkg-config glib-2.0 --cflags` \
              `pkg-config gtk+-2.0 --cflags`
LIBS = -Wall `pkg-config sqlite3 --libs` \
             `pkg-config glib-2.0 --libs` \
             `pkg-config gtk+-2.0 --libs`
if WIN32
LIBS += -mwindows
endif
