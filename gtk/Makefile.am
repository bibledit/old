SUBDIRS = man pix doc src scripts templates olpc desktop windows tests shutdown usfm kjv gobible xetex git
CLEANFILES = bibledit*.gz *~
EXTRA_DIST = TODO FDL

noinst_PROGRAMS = bumpversion
bumpversion_SOURCES = bumpversion.cpp

AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`

LIBS = -Wall `pkg-config glib-2.0 --libs`
