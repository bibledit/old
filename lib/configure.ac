AC_INIT([bibledit], [1.0.39],[http://www.nongnu.org/bibledit])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADER(config.h)

AC_PATH_PROG(GZIP_PATH, gzip, no)
if test x$GZIP_PATH = xno; then
  AC_MSG_ERROR(Program "gzip" is needed. Install this first.)
fi


AC_PATH_PROG(GUNZIP_PATH, gunzip, no)
if test x$GUNZIP_PATH = xno; then
  AC_MSG_ERROR(Program "gunzip" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_TAR, tar, no)
if test x$FIND_TAR = xno; then
  AC_MSG_ERROR(Program "tar" is needed. Install this first.)
fi


AC_PATH_PROG(ZIP_PATH, zip, no)
if test x$ZIP_PATH = xno; then
  AC_MSG_ERROR(Program "zip" is needed. Install this first.)
fi


AC_PATH_PROG(UNZIP_PATH, unzip, no)
if test x$UNZIP_PATH = xno; then
  AC_MSG_ERROR(Program "unzip" is needed. Install this first.)
fi


PKG_PROG_PKG_CONFIG
AC_PATH_PROG(PKG_CONFIG_PATH, pkg-config, no)
if test x$PKG_CONFIG_PATH = xno; then
  AC_MSG_ERROR(Program "pkg-config" is needed. Install this first.)
fi

AM_PATH_XML2(2.5.0, , AC_MSG_ERROR(Library libxml2-dev is needed. Install this first.))

PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(sqlite development version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

PKG_CHECK_MODULES(CURL, libcurl >= 7.20.0,,AC_MSG_ERROR(libcurl development version >= 7.20.0 is needed.))
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)
AC_CHECK_HEADER(curl/curl.h, , AC_MSG_ERROR([Header file curl/curl.h was not found. Usually this is in libcurl4-openssl-dev or libcurl4-gnutls-dev.]))

AC_DEFINE([DIRECTORY_SEPARATOR], ["/"], [Define the directory separator as Windows differs from Linux])

# Open installation.
AC_ARG_WITH(open-installation,
        [  --with-open-installation=bool
                          Open demo installation],
        [ OPEN_INSTALLATION=$withval ],
        [ OPEN_INSTALLATION=0 ]
)
AC_DEFINE([OPEN_INSTALLATION], [], [Define whether it is an open demo installation])
AC_DEFINE_UNQUOTED(OPEN_INSTALLATION, "$OPEN_INSTALLATION")
AC_SUBST(OPEN_INSTALLATION)

# Client installation.
AC_ARG_WITH(client-installation,
        [  --with-client-installation=bool
                          Client installation],
        [ CLIENT_INSTALLATION=$withval ],
        [ CLIENT_INSTALLATION=0 ]
)
AC_DEFINE([CLIENT_INSTALLATION], [], [Define whether it is a client installation])
AC_DEFINE_UNQUOTED(CLIENT_INSTALLATION, "$CLIENT_INSTALLATION")
AC_SUBST(CLIENT_INSTALLATION)

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
"
