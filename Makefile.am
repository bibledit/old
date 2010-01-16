SUBDIRS = man pix doc src scripts templates olpc desktop windows tests eeepc shutdown usfm kjv outpost gobible web dbus vcs xetex zend
# ACLOCAL_AMFLAGS = -I m4
CLEANFILES = bibledit*.gz *~
# EXTRA_DIST = m4/*

noinst_PROGRAMS = bumpversion
bumpversion_SOURCES = bumpversion.cpp

AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`

LIBS = -Wall `pkg-config glib-2.0 --libs`
