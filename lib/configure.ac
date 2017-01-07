AC_INIT([bibledit],[1.0.672],[http://bibledit.org])
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

PKG_CHECK_MODULES(CURL, libcurl >= 7.20.0,,AC_MSG_ERROR(libcurl development version >= 7.20.0 is needed.))
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)
AC_CHECK_HEADER(curl/curl.h, , AC_MSG_ERROR([Header file curl/curl.h was not found. Usually this is in libcurl4-openssl-dev or libcurl4-gnutls-dev.]))

PKG_CHECK_MODULES(OPENSSL, openssl >= 0.9.0,,AC_MSG_ERROR(openssl development version >= 0.9.0 is needed.))
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)


# Tag1 (leave in place)
# Tag2 (leave in place)
# Tag3 (leave in place)
# Tag4 (leave in place)


AC_CHECK_HEADER(pthread.h,AC_DEFINE([HAVE_PTHREAD],[1],[Define whether pthread.h is present]),)

AC_CHECK_HEADER(libproc.h,AC_DEFINE([HAVE_LIBPROC],[1],[Define whether libproc.h is present]),)

AC_CHECK_HEADER(execinfo.h,AC_DEFINE([HAVE_EXECINFO],[1],[Define whether execinfo.h is present]),)

AC_CHECK_HEADER(sys/sysctl.h,AC_DEFINE([HAVE_SYS_SYSCTL],[1],[Define whether sys/sysctl.h is present]),)

AC_CHECK_HEADER(mach/mach.h,AC_DEFINE([HAVE_MACH_MACH],[1],[Define whether mach/mach.h is present]),)


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

# Windows.
ENABLEWINDOWS=no
AC_ARG_ENABLE(windows,
[  --enable-windows Compile for Windows],
[
AC_DEFINE([WIN32], [1], [Define whether to compile on Windows])
AC_DEFINE([HAVE_WINDOWS], [1], [Define whether to compile on Windows])
ENABLEWINDOWS=yes
]
)

# Android.
ENABLEANDROID=no
AC_ARG_ENABLE(android,
[  --enable-android Compile for Android],
[
AC_DEFINE([HAVE_ANDROID], [1], [Define whether to compile for Android])
ENABLEANDROID=yes
]
)

# Mac.
ENABLEMAC=no
AC_ARG_ENABLE(mac,
[  --enable-mac Compile for Mac],
[
AC_DEFINE([HAVE_MAC], [1], [Define whether to compile for Mac])
ENABLEMAC=yes
]
)

# Linux.
ENABLELINUX=no
AC_ARG_ENABLE(linux,
[  --enable-linux Compile for Linux],
[
AC_DEFINE([HAVE_LINUX], [1], [Define whether to compile for Linux])
ENABLELINUX=yes
]
)

# iOS.
ENABLEIOS=no
AC_ARG_ENABLE(ios,
[  --enable-ios Compile for iOS],
[
AC_DEFINE([HAVE_IOS], [1], [Define whether to compile for iOS])
ENABLEIOS=yes
]
)

# Chrome OS.
ENABLECHROMEOS=no
AC_ARG_ENABLE(chromeos,
[  --enable-chromeos Compile for Chrome OS],
[
AC_DEFINE([HAVE_CHROMEOS], [1], [Define whether to compile for Chrome OS])
ENABLECHROMEOS=yes
]
)

# Chrome App.
ENABLECHROMEAPP=no
AC_ARG_ENABLE(chromeapp,
[  --enable-chromeapp Compile for Chrome App],
[
AC_DEFINE([HAVE_CHROMEAPP], [1], [Define whether to compile for Chrome App])
ENABLECHROMEAPP=yes
]
)

AC_OUTPUT(Makefile)

echo "
Bibledit $VERSION configuration summary:

Installation prefix             : ${prefix}
Unit tests (--enable-unittests) : ${UNITTESTS}
Windows    (--enable-windows)   : ${ENABLEWINDOWS}
Android    (--enable-android)   : ${ENABLEANDROID}
Mac        (--enable-mac)       : ${ENABLEMAC}
Linux      (--enable-linux)     : ${ENABLELINUX}
iOS        (--enable-ios)       : ${ENABLEIOS}
Chrome OS  (--enable-chromeos)  : ${ENABLECHROMEOS}
Chrome App (--enable-chromeapp) : ${ENABLECHROMEAPP}
"
