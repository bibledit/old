SUBDIRS = man pix doc src scripts templates olpc desktop windows tests eeepc shutdown usfm kjv outpost gobible xetex git
CLEANFILES = bibledit*.gz *~
EXTRA_DIST = TODO

noinst_PROGRAMS = bumpversion
bumpversion_SOURCES = bumpversion.cpp

AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`

LIBS = -Wall `pkg-config glib-2.0 --libs`
