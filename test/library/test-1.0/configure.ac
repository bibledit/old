AC_INIT([test],[1.0],[http://test.com])
AM_INIT_AUTOMAKE
AC_CANONICAL_BUILD
ac_default_prefix=/usr

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADER(config.h)

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

AC_OUTPUT(Makefile)
