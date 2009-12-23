SUBDIRS = man pix doc src scripts templates olpc desktop windows tests eeepc browser shutdown usfm kjv lexicons outpost gobible web dbus
# ACLOCAL_AMFLAGS = -I m4
CLEANFILES = bibledit*.gz *~
# EXTRA_DIST = m4/*

noinst_PROGRAMS = bumpversion
bumpversion_SOURCES = bumpversion.cpp

AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`

LIBS = -Wall `pkg-config glib-2.0 --libs`
