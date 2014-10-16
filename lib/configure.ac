AC_INIT([bibledit], [1.0.1],[http://www.nongnu.org/bibledit])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADER(config.h)

AM_PATH_XML2(2.5.0, , AC_MSG_ERROR(Library libxml2-dev is needed. Install this first.))

PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(sqlite development version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
"
