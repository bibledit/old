#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ(2.63)
AC_INIT([bibledit-gtk],[4.8.1],[http://www.nongnu.org/bibledit])
AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
ac_default_prefix=/usr
AC_CONFIG_SRCDIR([src])
AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_HEADERS([config.h])


CXXFLAGS="-g -O0 ${CXXFLAGS}"
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CC
AC_HEADER_STDC
PKG_CHECK_MODULES(GTK, gtk+-2.0 gdk-2.0,,AC_MSG_ERROR(Gtk2 development version is needed.))
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)


PKG_PROG_PKG_CONFIG


AC_PATH_PROG(PKG_CONFIG_PATH, pkg-config, no)
if test x$PKG_CONFIG_PATH = xno; then
  AC_MSG_ERROR(Program "pkg-config" is needed. Install this first.)
fi


AC_PATH_PROG(GREP_PATH, grep, no)
if test x$GREP_PATH = xno; then
  AC_MSG_ERROR(Program "grep" is needed. Install this first.)
fi


AC_PATH_PROG(TEE_TOUCH, touch, no)
if test x$TOUCH_PATH = xno; then
  AC_MSG_ERROR(Program "touch" is needed. Install this first.)
fi


AC_PATH_PROG(DATE_PATH, date, no)
if test x$DATE_PATH = xno; then
  AC_MSG_ERROR(Program "date" is needed. Install this first.)
fi


AC_PATH_PROG(TAIL_PATH, tail, no)
if test x$TAIL_PATH = xno; then
  AC_MSG_ERROR(Program "tail" is needed. Install this first.)
fi


AC_PATH_PROG(GZIP_PATH, gzip, no)
if test x$GZIP_PATH = xno; then
  AC_MSG_ERROR(Program "gzip" is needed. Install this first.)
fi


AC_PATH_PROG(GUNZIP_PATH, gunzip, no)
if test x$GUNZIP_PATH = xno; then
  AC_MSG_ERROR(Program "gunzip" is needed. Install this first.)
fi


AC_PATH_PROG(ICONV_PATH, iconv, no)
if test x$ICONV_PATH = xno; then
  AC_MSG_ERROR(Program "iconv" is needed. Install this first.)
fi


AC_PATH_PROG(STRINGS_PATH, strings, no)
if test x$STRINGS_PATH = xno; then
  AC_MSG_ERROR(Program "strings" is needed. Install this first.)
fi


AC_PATH_PROG(CAT_PATH, cat, no)
if test x$CAT_PATH = xno; then
  AC_MSG_ERROR(Program "cat" is needed. Install this first.)
fi


AC_PATH_PROG(SORT_PATH, sort, no)
if test x$SORT_PATH = xno; then
  AC_MSG_ERROR(Program "sort" is needed. Install this first.)
fi


AC_PATH_PROG(HEAD_PATH, head, no)
if test x$HEAD_PATH = xno; then
  AC_MSG_ERROR(Program "head" is needed. Install this first.)
fi


AC_PATH_PROG(MAKE_PATH, make, no)
if test x$MAKE_PATH = xno; then
  AC_MSG_ERROR(Program "make" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_PATH, find, no)
if test x$FIND_PATH = xno; then
  AC_MSG_ERROR(Program "find" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_XARGS, xargs, no)
if test x$FIND_XARGS = xno; then
  AC_MSG_ERROR(Program "xargs" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_GPP, g++, no)
if test x$FIND_GPP = xno; then
  AC_MSG_ERROR(Program "g++" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_DIFF, diff, no)
if test x$FIND_DIFF = xno; then
  AC_MSG_ERROR(Program "diff" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_PATCH, patch, no)
if test x$FIND_PATCH = xno; then
  AC_MSG_ERROR(Program "patch" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_MERGE, merge, no)
if test x$FIND_MERGE = xno; then
  AC_MSG_ERROR(Program "merge" (in package rcs) is needed. Install this first.)
fi


AC_PATH_PROG(FIND_TAR, tar, no)
if test x$FIND_TAR = xno; then
  AC_MSG_ERROR(Program "tar" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_CURL, curl, no)
if test x$FIND_CURL = xno; then
  AC_MSG_ERROR(Program "curl" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_PHP, php, no)
if test x$FIND_PHP = xno; then
  AC_MSG_ERROR(Program "php" (in package php5-cli) is needed. Install this first.)
fi


PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16.0,,AC_MSG_ERROR(glib development version >= 2.16.0 is needed.))
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)


PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.12.0,,AC_MSG_ERROR(gtk+ development version >= 2.12.0 is needed.))
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)


PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16.0,,AC_MSG_ERROR(gthread development version >= 2.16.0 is needed.))
AC_SUBST(GTHREAD_CFLAGS)
AC_SUBST(GTHREAD_LIBS)


PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= 2.16.0,,AC_MSG_ERROR(gobject development version >= 2.16.0 is needed.))
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)


PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(sqlite development version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)


AM_PATH_XML2(2.5.0, , AC_MSG_ERROR(Library libxml2-dev is needed. Install this first.))


PKG_CHECK_MODULES(ENCHANT, enchant >= 1.2.0)
AC_SUBST(ENCHANT_CFLAGS)
AC_SUBST(ENCHANT_LIBS)


PKG_CHECK_MODULES(PANGO, [pango >= 1.3.5],
   [AC_DEFINE(HAVE_PANGO_UNDERLINE_ERROR, 1, [Define to 1 if PANGO_UNDERLINE_ERROR is available.])],
   [AC_MSG_WARN([Disabling new error underlining since pango < 1.3.5])])


PKG_CHECK_MODULES(PANGO, [pango >= 1.8.0],
   [AC_DEFINE(FIXED_PANGO, 1, [Define to 1 if pango >= 1.8.0.])],
   [AC_MSG_WARN([Enabling word-breaking workarounds since pango < 1.8.0])])


PKG_CHECK_MODULES(GTKSOURCEVIEW, gtksourceview-2.0 >= 2.2.0)
AC_SUBST(GTKSOURCEVIEW_CFLAGS)
AC_SUBST(GTKSOURCEVIEW_LIBS)


PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.0.0,,AC_MSG_ERROR(libwebkit development version >= 1.0.0 is needed.))
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)


PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4 >= 2.4,,AC_MSG_ERROR(libsoup-2.4 development version >= 2.4 is needed.))
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)


AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))


# This code is kept to learn from.
# AC_CHECK_HEADER(aspell.h, [AC_DEFINE(HAVE_ASPELL_H,1, 
#    [Define to 1 if you have the <aspell.h> header file.] )] )
# AC_CHECK_HEADER(pspell/pspell.h, [AC_DEFINE(HAVE_PSPELL_H,1, 
#    [Define to 1 if you have the <pspell/pspell.h> header file.] )] )
# AC_SUBST(SPELLER_LIB)


if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi


AC_MSG_CHECKING([whether to enable LINUX specific flags])
case "$host_os" in
*linux*)
	linux=true
	AC_MSG_RESULT([yes])
  AC_DEFINE_UNQUOTED(LINUX, "1",[LINUX])
	;;
*)
	linux=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(LINUX, test x$linux = xtrue)


AC_MSG_CHECKING([whether to enable DARWIN specific flags])
case "$host_os" in
*darwin*)
	darwin=true
	AC_MSG_RESULT([yes])
  AC_DEFINE_UNQUOTED(DARWIN, "1",[DARWIN])
	;;
*)
	darwin=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(DARWIN, test x$darwin = xtrue)


AC_MSG_CHECKING([whether to enable WIN32 specific flags])
case "$host_os" in
*mingw*)
	win32=true
	AC_MSG_RESULT([yes])
  AC_DEFINE_UNQUOTED(WIN32, "1",[WIN32])
	;;
*)
	win32=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)


packagedatadir=share/${PACKAGE}
packagedocdir=doc/${PACKAGE}
AC_SUBST(packagedatadir)


packagepixmapsdir=${packagedatadir}/pixmaps
packagehelpdir=${packagedatadir}/help
packagemenudir=${packagedatadir}
packagelocalstatedir=var/${PACKAGE}


if test "x${datarootdir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}",[PACKAGE_DATA_DIR])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}",[PACKAGE_DATA_DIR])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datarootdir}/${PACKAGE}",[PACKAGE_DATA_DIR])
fi 


AC_CONFIG_FILES([Makefile 
		  man/Makefile 
		  pix/Makefile 
		  doc/Makefile 
		  src/Makefile 
		  scripts/Makefile
		  templates/Makefile
		  olpc/Makefile
		  desktop/Makefile
		  windows/Makefile
		  tests/Makefile
		  shutdown/Makefile
		  usfm/Makefile
		  kjv/Makefile
		  xetex/Makefile
		  git/Makefile
		  php/Makefile
		])
AC_OUTPUT


echo "
Bibledit-Gtk $VERSION configuration summary:

  Host operating system  : ${host_os}
  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
"
