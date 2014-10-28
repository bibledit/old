AC_INIT([bibledit-linux], [1.0.1],[http://www.nongnu.org/bibledit])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([subdir-objects tar-ustar])
AC_CONFIG_HEADER(config.h)

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(GTK, gtk+-3.0,,AC_MSG_ERROR(Gtk3 development version is needed.))
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

AC_PATH_PROG(PKG_CONFIG_PATH, pkg-config, no)
if test x$PKG_CONFIG_PATH = xno; then
  AC_MSG_ERROR(Program "pkg-config" is needed. Install this first.)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
Bibledit-Linux $VERSION configuration summary:

  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
"
