dist_pkgdata_DATA = *.html *.css *.jpeg *.gif
EXTRA_DIST = update dot.mozconfig FDL
CLEANFILES = *~
noinst_PROGRAMS = allpages contents
allpages_SOURCES = allpages.cpp
contents_SOURCES = contents.cpp
AM_CXXFLAGS = -Wall \
              `pkg-config gobject-2.0 --cflags` \
              `pkg-config glib-2.0 --cflags` \
              `pkg-config gtk+-2.0 --cflags`
LIBS = -Wall `pkg-config gobject-2.0 --libs` \
             `pkg-config glib-2.0 --libs` \
             `pkg-config gtk+-2.0 --libs`

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
