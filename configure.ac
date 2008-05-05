#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(bibledit, 3.2, http://www.nongnu.org/bibledit)
AC_CANONICAL_SYSTEM
ac_default_prefix=/usr
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_ISC_POSIX
AM_PROG_CC_STDC
AC_PROG_RANLIB
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

AC_PATH_PROG(FIND_PING, ping, no)
if test x$FIND_PING = xno; then
  AC_MSG_ERROR(Program "ping" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_XARGS, xargs, no)
if test x$FIND_XARGS = xno; then
  AC_MSG_ERROR(Program "xargs" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_GIT, git, no)
if test x$FIND_GIT = xno; then
  AC_MSG_ERROR(Program "git" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_GPP, g++, no)
if test x$FIND_GPP = xno; then
  AC_MSG_ERROR(Program "g++" is needed. Install this first.)
fi

# Checks for libraries.

PKG_CHECK_MODULES(fontconfig, fontconfig,,AC_MSG_ERROR(fontconfig development version is needed.))
pkg_modules="fontconfig >= 1.0.1"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

pkg_modules="glib-2.0 >= 2.8.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

pkg_modules="gtk+-2.0 >= 2.6.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

pkg_modules="gthread-2.0 >= 2.4.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Library sqlite3.
# Note that we use only the library part of package sqlite3.
# The commandline tool "sqlite3" is not used because some distributions
# do not include it.
pkg_modules="sqlite3 >= 3.0.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])

# Check for availability of libxml2
AM_PATH_XML2(2.5.0, , AC_MSG_ERROR(Library libxml2-dev is needed. Install this first.))

# PKG_CHECK_MODULES(dbus-dev, [dbus-1 >= 0.70])

PKG_CHECK_MODULES(ENCHANT, enchant >= 1.3.0)
AC_SUBST(ENCHANT_CFLAGS)
AC_SUBST(ENCHANT_LIBS)

# check Pango version so we can use PANGO_UNDERLINE_ERROR when available:
PKG_CHECK_MODULES(PANGO, [pango >= 1.3.5],
   [AC_DEFINE(HAVE_PANGO_UNDERLINE_ERROR, 1, [Define to 1 if PANGO_UNDERLINE_ERROR is available.])],
   [AC_MSG_WARN([Disabling new error underlining since pango < 1.3.5])])

# check Pango version to see if word breaking bug is fixed:
PKG_CHECK_MODULES(PANGO, [pango >= 1.8.0],
   [AC_DEFINE(FIXED_PANGO, 1, [Define to 1 if pango >= 1.8.0.])],
   [AC_MSG_WARN([Enabling word-breaking workarounds since pango < 1.8.0])])


dnl  *****
dnl  Gecko
dnl  *****
GECKO_INIT([GECKO], [gecko_found=yes],[gecko_found=no])
AC_SUBST([GECKO])
AC_SUBST([GECKO_FLAVOUR])
AC_SUBST([GECKO_INCLUDE_ROOT])
AC_SUBST([GECKO_HOME])
AC_SUBST([GECKO_PREFIX])
AC_SUBST([GECKO_EXTRA_LIBS])
AC_SUBST([GECKO_GLUE_LIBS])

case "$gecko_cv_gecko" in
xulrunner) min_version=1.8 ;;
*firefox) min_version=1.5 ;;
*) AC_MSG_ERROR([Unsupported gecko "$gecko_cv_gecko"]) ;;
esac

# Added $gecko_cv_gecko-js for debian xulrunner brokenness
PKG_CHECK_MODULES([GECKO],[${gecko_cv_gecko}-xpcom >= $min_version ${gecko_cv_gecko}-js $gecko_cv_extra_pkg_dependencies])
AC_SUBST([GECKO_CFLAGS])
AC_SUBST([GECKO_LIBS])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h unistd.h sys/uio.h)
AC_HEADER_DIRENT
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h limits.h locale.h netdb.h netinet/in.h stddef.h stdlib.h string.h sys/file.h sys/socket.h sys/time.h unistd.h])
AC_CHECK_HEADERS([sys/ioctl.h])
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

# This code is kept to learn from.
# AC_CHECK_HEADER(aspell.h, [AC_DEFINE(HAVE_ASPELL_H,1, 
#    [Define to 1 if you have the <aspell.h> header file.] )] )
# AC_CHECK_HEADER(pspell/pspell.h, [AC_DEFINE(HAVE_PSPELL_H,1, 
#    [Define to 1 if you have the <pspell/pspell.h> header file.] )] )
# AC_SUBST(SPELLER_LIB)
# GTKSPELL_PACKAGES=gtk+-2.0
# AC_SUBST(GTKSPELL_PACKAGES)
# PKG_CHECK_MODULES(GTKSPELL, $GTKSPELL_PACKAGES)
# AC_SUBST(GTKSPELL_CFLAGS)
# AC_SUBST(GTKSPELL_LIBS)


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

AC_SUBST(src doc bin util lib)

# Checks for library functions.
AC_CHECK_FUNCS([strdup strstr])
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit dup2 ftruncate inet_ntoa memchr memmove memset mkdir rint select setlocale socket strchr strcspn strpbrk strrchr strstr strtol])
AC_CHECK_FUNCS([gethostbyname gethostbyaddr])
AC_CHECK_FUNCS([endgrent endpwent getcwd gethostname strerror tzset mkfifo stpcpy])
AC_CHECK_HEADERS([sys/param.h])
AC_FUNC_CHOWN
AC_FUNC_LSTAT
AC_TYPE_UID_T
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL

# Checks for Additional stuffs.
# Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi

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
AC_OUTPUT( Makefile 
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
                   eeepc/Makefile
		   )

echo "
Bibledit $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        Gecko home               : ${GECKO_HOME}
"
