AC_INIT([bibledit], [1.0.61],[http://www.nongnu.org/bibledit])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
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

PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(libsqlite3-dev version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

PKG_CHECK_MODULES(CURL, libcurl >= 7.20.0,,AC_MSG_ERROR(libcurl development version >= 7.20.0 is needed.))
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)
AC_CHECK_HEADER(curl/curl.h, , AC_MSG_ERROR([Header file curl/curl.h was not found. Usually this is in libcurl4-openssl-dev or libcurl4-gnutls-dev.]))

# Libgit path.
AC_ARG_WITH(libgit-path,
  [  --with-libgit-path=path Path to the libgit2 headers and library],
  [ LIBGIT_PATH=$withval ],
  [ LIBGIT_PATH=undefined ]
)
AC_DEFINE([LIBGIT_PATH], [], [The path to libgit2])
AC_DEFINE_UNQUOTED(LIBGIT_PATH, "$LIBGIT_PATH")
AC_SUBST(LIBGIT_PATH)
AS_IF([test -d ${LIBGIT_PATH}], [
    AC_MSG_CHECKING([GIT_CFLAGS])
    GIT_CFLAGS=-I${LIBGIT_PATH}/include
    AC_MSG_RESULT([${GIT_CFLAGS}])
    AC_MSG_CHECKING([GIT_LIBS])
    GIT_LIBS="-L${LIBGIT_PATH}/lib -lgit2"
    AC_MSG_RESULT([${GIT_LIBS}])
  ],
  AC_MSG_CHECKING([included libgit2])
  AC_MSG_RESULT([no])
  AC_MSG_NOTICE([the two messages "command not found" are just cosmetic])
  PKG_CHECK_MODULES(GIT, libgit2 >= 0.20.0,,
    AC_MSG_NOTICE([the following error may be fixed in several ways:])
    AC_MSG_NOTICE([* ./configure --with-libgit-path=../dependencies/libgit2/osx/x86_64])
    AC_MSG_NOTICE([* install package libgit2-dev version 0.21 exactly])
    AC_MSG_ERROR([libgit2 cannot be located and is needed])
  )
)
AC_SUBST(GIT_CFLAGS)
AC_SUBST(GIT_LIBS)
# Fails for unknown reason: AC_CHECK_HEADER(git2.h, , AC_MSG_ERROR([Header file git2.h was not found. Usually this is in libgit2-dev.]))

PKG_CHECK_MODULES(OPENSSL, openssl >= 0.9.0,,AC_MSG_ERROR(openssl development version >= 0.9.0 is needed.))
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

AC_DEFINE([DIRECTORY_SEPARATOR], ["/"], [Define the directory separator as Windows differs from Linux])

# Demo installation.
DEMO=no
AC_ARG_ENABLE(demo,
[  --enable-demo  Enable demo mode],
[ DEMO=yes
])
AC_DEFINE([DEMO], [], [Define whether it is a demo installation])
AC_DEFINE_UNQUOTED(DEMO, "$DEMO")
AC_SUBST(DEMO)

# Client installation.
CLIENT=no
AC_ARG_ENABLE(client,
  [--enable-client Enable client mode],
  [CLIENT=yes]
)
AC_DEFINE([CLIENT], [], [Define whether it is a client installation])
AC_DEFINE_UNQUOTED(CLIENT, "$CLIENT")
AC_SUBST(CLIENT)

# Network port.
AC_ARG_WITH(network-port,
[  --with-network-port=number Port number for server to listen on],
[ NETWORK_PORT=$withval ],
[ NETWORK_PORT=8080 ]
)
AC_DEFINE([NETWORK_PORT], [], [Define the network port number])
AC_DEFINE_UNQUOTED(NETWORK_PORT, "$NETWORK_PORT")
AC_SUBST(NETWORK_PORT)

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

  Version                : ${PACKAGE_VERSION}
  Installation prefix    : ${prefix}
  Libgit2 include        : ${GIT_CFLAGS}
  Libgit2 library        : ${GIT_LIBS}
  Network port           : ${NETWORK_PORT}
  Client installation    : ${CLIENT}
  Demo installation      : ${DEMO}
"
