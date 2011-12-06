#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([bibledit-bibletime],[1.1.1],[http://www.nongnu.org/bibledit])
# AC_CANONICAL_TARGET
ac_default_prefix=/usr
AC_CONFIG_SRCDIR([.])
AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
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

AC_PATH_PROG(MAKE_PATH, make, no)
if test x$MAKE_PATH = xno; then
  AC_MSG_ERROR(Program "make" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_GPP, g++, no)
if test x$FIND_GPP = xno; then
  AC_MSG_ERROR(Program "g++" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_CURL, curl, no)
if test x$FIND_CURL = xno; then
  AC_MSG_ERROR(Program "curl" is needed. Install this first.)
fi

# Checks for libraries.

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

PKG_CHECK_MODULES(DBUSGLIB, dbus-glib-1 >= 0.7.0,,AC_MSG_ERROR(libdbus-glib-1 development version >= 0.7.0 is needed.))
AC_SUBST(DBUSGLIB_CFLAGS)
AC_SUBST(DBUSGLIB_LIBS)

PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4 >= 2.4,,AC_MSG_ERROR(libsoup-2.4 development version >= 2.4 is needed.))
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)

# Checks for Additional stuffs.
# Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi


# LINUX build checks
AC_MSG_CHECKING([whether to enable LINUX specific flags])
case "$host_os" in
*linux*)
	linux=true
	AC_MSG_RESULT([yes])
	;;
*)
	linux=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(LINUX, test x$linux = xtrue)

# DARWIN build checks
AC_MSG_CHECKING([whether to enable DARWIN specific flags])
case "$host_os" in
*darwin*)
	darwin=true
	AC_MSG_RESULT([yes])
	;;
*)
	darwin=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(DARWIN, test x$darwin = xtrue)

# WIN32 build checks
AC_MSG_CHECKING([whether to enable WIN32 specific flags])
case "$host_os" in
*mingw*)
	win32=true
	AC_MSG_RESULT([yes])
	;;
*)
	win32=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)


# Set PACKAGE DATA & DOC DIR
packagedatadir=share/${PACKAGE}
packagedocdir=doc/${PACKAGE}
AC_SUBST(packagedatadir)


# Set PACKAGE DIRS in config.h.
packagepixmapsdir=${packagedatadir}/pixmaps
packagehelpdir=${packagedatadir}/help
packagemenudir=${packagedatadir}
packagelocalstatedir=var/${PACKAGE}

# Subst PACKAGE_DATA_DIR.
if test "x${datarootdir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}",[PACKAGE_DATA_DIR])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}",[PACKAGE_DATA_DIR])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datarootdir}/${PACKAGE}",[PACKAGE_DATA_DIR])
fi 

# Create all the Makefiles.
AC_CONFIG_FILES([Makefile man/Makefile])
AC_OUTPUT

# Summary.
echo "
Bibledit-BibleTime $VERSION configuration summary:

  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
"
