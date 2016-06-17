AC_INIT([bibledit],[1.0.475],[http://bibledit.org])
AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([tar-ustar subdir-objects])
AC_CANONICAL_BUILD
ac_default_prefix=/usr

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_RANLIB
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


AC_PATH_PROG(INSTALLMGR_PATH, installmgr, no)
AC_PATH_PROG(DIATHEKE_PATH, diatheke, no)


PKG_PROG_PKG_CONFIG
AC_PATH_PROG(PKG_CONFIG_PATH, pkg-config, no)
if test x$PKG_CONFIG_PATH = xno; then
  AC_MSG_ERROR(Program "pkg-config" is needed. Install this first.)
fi

PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(libsqlite3-dev version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

PKG_CHECK_MODULES(CURL, libcurl >= 7.20.0,,AC_MSG_ERROR(libcurl development version >= 7.20.0 is needed.))
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)
AC_CHECK_HEADER(curl/curl.h, , AC_MSG_ERROR([Header file curl/curl.h was not found. Usually this is in libcurl4-openssl-dev or libcurl4-gnutls-dev.]))

PKG_CHECK_MODULES(OPENSSL, openssl >= 0.9.0,,AC_MSG_ERROR(openssl development version >= 0.9.0 is needed.))
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

PKG_CHECK_MODULES(SWORD16, sword < 1.7.0, [AC_DEFINE([HAVE_SWORD16],[1],[Define whether the SWORD library 1.6.x is present])], [AC_MSG_RESULT()])

PKG_CHECK_MODULES(SWORD, sword >= 1.6.0, [AC_DEFINE([HAVE_SWORD],[1],[Define whether the SWORD library is present])], [AC_MSG_RESULT([no])])
AC_SUBST(SWORD_CFLAGS)
AC_SUBST(SWORD_LIBS)


# Tag1 (leave in place)
# Tag2 (leave in place)
# Tag3 (leave in place)
# Tag4 (leave in place)


AC_CHECK_HEADER(pthread.h,AC_DEFINE([HAVE_PTHREAD],[1],[Define whether pthread.h is present]),)

AC_CHECK_HEADER(libproc.h,AC_DEFINE([HAVE_LIBPROC],[1],[Define whether libproc.h is present]),)

AC_CHECK_HEADER(execinfo.h,AC_DEFINE([HAVE_EXECINFO],[1],[Define whether execinfo.h is present]),)

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

# Check whether function 'stoi' is available
AC_MSG_CHECKING([whether stoi is available])
AC_COMPILE_IFELSE(
[AC_LANG_SOURCE([[
#include <string>
using namespace std;
int i = stoi ("string");
  ]])],
  [AC_DEFINE([HAVE_STOI], [1], [Define whether function 'stoi' is available])
   AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no])]
)

# Enable URL settings.
URLSETTINGS=no
AC_ARG_ENABLE(urlsettings,
[  --enable-urlsettings Enable URL settings],
[AC_DEFINE([HAVE_URLSETTINGS], [1], [Define whether to enable URL settings])]
URLSETTINGS=yes
)
AC_DEFINE([URLSETTINGS], [], [Define whether URL settings are enabled])
AC_DEFINE_UNQUOTED(URLSETTINGS, "$URLSETTINGS")
AC_SUBST(URLSETTINGS)

# Enable unit tests.
UNITTESTS=no
AC_ARG_ENABLE(unittests,
[  --enable-unittests Enable unit tests],
[AC_DEFINE([HAVE_UNITTESTS], [1], [Define whether to enable unit tests])]
UNITTESTS=yes
)
AC_DEFINE([UNITTESTS], [], [Define whether unit tests are enabled])
AC_DEFINE_UNQUOTED(UNITTESTS, "$UNITTESTS")
AC_SUBST(UNITTESTS)

# Configure admin username.
AC_ARG_WITH(admin-username,
[  --with-admin-username=username Admin username],
[ ADMIN_USERNAME=$withval ],
[ ADMIN_USERNAME= ]
)
AC_DEFINE([ADMIN_USERNAME], [], [Define the admin username])
AC_DEFINE_UNQUOTED(ADMIN_USERNAME, "$ADMIN_USERNAME")
AC_SUBST(ADMIN_USERNAME)

# Configure admin password.
AC_ARG_WITH(admin-password,
[  --with-admin-password=password Admin password],
[ ADMIN_PASSWORD=$withval ],
[ ADMIN_PASSWORD= ]
)
AC_DEFINE([ADMIN_PASSWORD], [], [Define the admin password])
AC_DEFINE_UNQUOTED(ADMIN_PASSWORD, "$ADMIN_PASSWORD")
AC_SUBST(ADMIN_PASSWORD)

# Configure admin email.
AC_ARG_WITH(admin-email,
[  --with-admin-email=email Admin email],
[ ADMIN_EMAIL=$withval ],
[ ADMIN_EMAIL= ]
)
AC_DEFINE([ADMIN_EMAIL], [], [Define the admin email])
AC_DEFINE_UNQUOTED(ADMIN_EMAIL, "$ADMIN_EMAIL")
AC_SUBST(ADMIN_EMAIL)

# Enable tiny journal.
TINYJOURNAL=no
AC_ARG_ENABLE(tinyjournal,
[  --enable-tinyjournal Enable tiny journal],
[AC_DEFINE([HAVE_TINYJOURNAL], [1], [Define whether to enable tiny journal])]
TINYJOURNAL=yes
)
AC_DEFINE([TINYJOURNAL], [], [Define whether the tiny journal is enabled])
AC_DEFINE_UNQUOTED(TINYJOURNAL, "$TINYJOURNAL")
AC_SUBST(TINYJOURNAL)

# Enable Paratext synchronization.
PARATEXT=no
AC_ARG_ENABLE(paratext,
[  --enable-paratext Enable Paratext synchronization],
[AC_DEFINE([HAVE_PARATEXT], [1], [Define whether to enable Paratext synchronization])]
PARATEXT=yes
)
AC_DEFINE([PARATEXT], [], [Define whether Paratext synchronization is enabled])
AC_DEFINE_UNQUOTED(PARATEXT, "$PARATEXT")
AC_SUBST(PARATEXT)

# Configure resources path.
AC_ARG_WITH(resources-path,
[  --with-resources-path=path Resources path],
[ RESOURCES_PATH=$withval ],
[ RESOURCES_PATH= ]
)
AC_DEFINE([RESOURCES_PATH], [], [Define the path to the shared external resources cache])
AC_DEFINE_UNQUOTED(RESOURCES_PATH, "$RESOURCES_PATH")
AC_SUBST(RESOURCES_PATH)

# Windows.
WINDOWS=no
AC_ARG_ENABLE(windows,
[  --enable-windows Compile on Windows],
[AC_DEFINE([WIN32], [1], [Define whether to compile on Windows])]
WINDOWS=yes
)
AC_DEFINE([WINDOWS], [], [Define whether to compile on Windows])
AC_DEFINE_UNQUOTED(WINDOWS, "$WINDOWS")
AC_SUBST(WINDOWS)

# Android.
ENABLEANDROID=no
AC_ARG_ENABLE(android,
[  --enable-android Compile for Android],
[AC_DEFINE([HAVE_ANDROID], [1], [Define whether to compile for Android])]
ENABLEANDROID=yes
)

# iOS.
ENABLEIOS=no
AC_ARG_ENABLE(ios,
[  --enable-ios Compile for iOS],
[AC_DEFINE([HAVE_IOS], [1], [Define whether to compile for iOS])]
ENABLEIOS=yes
)

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

Installation prefix                               : ${prefix}
Network port (--with-network-port=n)              : ${NETWORK_PORT}
Client edition (--enable-client)                  : ${CLIENT}
Demo edition (--enable-demo)                      : ${DEMO}
Maximum number of parallel tasks
                       (--with-parallel-tasks=n)  : ${PARALLEL_TASKS}
Bare browser mode (--enable-bare-browser)         : ${BARE_BROWSER}
URL settings (--enable-urlsettings)               : ${URLSETTINGS}
Unit tests (--enable-unittests)                   : ${UNITTESTS}
Admin username (--with-admin-username=username)   : ${ADMIN_USERNAME}
Admin password (--with-admin-password=password)   : ${ADMIN_PASSWORD}
Admin email (--with-admin-email=email)            : ${ADMIN_EMAIL}
Tiny journal (--enable-tinyjournal)               : ${TINYJOURNAL}
Paratext synchronization (--enable-paratext)      : ${PARATEXT}
Resources cache path (--with-resources-path=path) : ${RESOURCES_PATH}
Windows (--enable-windows)                        : ${WINDOWS}
Android (--enable-android)                        : ${ENABLEANDROID}
iOS (--enable-ios)                                : ${ENABLEIOS}
SWORD library                                     : ${SWORD_CFLAGS}
"
