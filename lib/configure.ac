AC_INIT([bibledit], [1.0.90],[http://www.nongnu.org/bibledit])
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

# Check for libgit2.a in the source directory.
AC_MSG_CHECKING([libgit2])
LIBGIT=libgit2.a
AS_IF([test -f ${LIBGIT}], [
    GIT_LIBS="-lz"
    AC_DEFINE([HAVE_GIT], [1], [Whether git funcions are available and enabled])
    AC_MSG_RESULT([$LIBGIT])
  ],
  AC_MSG_RESULT([no])
  LIBGIT=""
)
AC_SUBST(GIT_LIBS)
AC_SUBST(LIBGIT)
AC_SUBST(GIT_ENABLED)

PKG_CHECK_MODULES(OPENSSL, openssl >= 0.9.0,,AC_MSG_ERROR(openssl development version >= 0.9.0 is needed.))
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

AC_CHECK_HEADER(pthread.h,AC_DEFINE([HAVE_PTHREAD],[1],[Define whether pthread.h is present]),)

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
  [  --enable-client Enable client mode],
  [AC_DEFINE([CLIENT_PREPARED], [1], [Define whether it is a client installation])]
  CLIENT=yes
)
AC_DEFINE([CLIENT], [], [Define whether it is prepared as a client installation])
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

# Maximum number of parallel tasks to run.
AC_ARG_WITH(parallel-tasks,
[  --with-parallel-tasks=number Maximum number of parallel background tasks],
[ PARALLEL_TASKS=$withval ],
[ PARALLEL_TASKS=10 ]
)
AC_DEFINE([PARALLEL_TASKS], [], [Define the maximum number of parallel background tasks to run])
AC_DEFINE_UNQUOTED(PARALLEL_TASKS, $PARALLEL_TASKS)
AC_SUBST(PARALLEL_TASKS)

# Bare browser mode.
BARE_BROWSER=no
AC_ARG_ENABLE(bare-browser,
[  --enable-bare-browser Enable bare browser mode],
[BARE_BROWSER=yes]
)
AC_DEFINE([BARE_BROWSER], [], [Define whether the expected browser is a bare browser without controls])
AC_DEFINE_UNQUOTED(BARE_BROWSER, "$BARE_BROWSER")
AC_SUBST(BARE_BROWSER)

# Build for iOS.
IOS=no
AC_ARG_ENABLE(ios,
  [  --enable-ios Enable iOS],
  [AC_DEFINE([HAVE_IOS], [1], [Define whether to build for iOS])]
  IOS=yes
)
AC_DEFINE([IOS], [], [Define whether it is to be built for iOS])
AC_DEFINE_UNQUOTED(IOS, "$IOS")
AC_SUBST(IOS)

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

Installation prefix                   : ${prefix}
Libgit2                               : ${LIBGIT}
Network port                          : ${NETWORK_PORT}
Client installation (--enable-client) : ${CLIENT}
Demo installation (--enable-demo)     : ${DEMO}
Maximum number of parallel tasks      : ${PARALLEL_TASKS}
Bare browser mode                     : ${BARE_BROWSER}
Build for iOS (--enable-ios)          : ${IOS}
"
