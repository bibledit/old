dist_pkgdata_DATA = *.html *.css *.jpeg *.gif
EXTRA_DIST = update GFDL
CLEANFILES = *~
noinst_PROGRAMS = allpages contents
allpages_SOURCES = allpages.cpp
contents_SOURCES = contents.cpp
AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`
LIBS = -Wall `pkg-config glib-2.0 --libs`
