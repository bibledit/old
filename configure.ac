#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([bibledit],[4.0.59],[http://www.nongnu.org/bibledit])
# AC_CANONICAL_TARGET
ac_default_prefix=/usr
# AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
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
  AC_MSG_ERROR(Program "git" (usually called git-core) is needed. Install this first.)
fi

AC_PATH_PROG(FIND_GPP, g++, no)
if test x$FIND_GPP = xno; then
  AC_MSG_ERROR(Program "g++" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_DIFF, diff, no)
if test x$FIND_DIFF = xno; then
  AC_MSG_ERROR(Program "diff" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_PATCH, patch, no)
if test x$FIND_PATCH = xno; then
  AC_MSG_ERROR(Program "patch" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_MERGE, merge, no)
if test x$FIND_MERGE = xno; then
  AC_MSG_ERROR(Program "merge" (in package rcs) is needed. Install this first.)
fi

AC_PATH_PROG(FIND_TAR, tar, no)
if test x$FIND_TAR = xno; then
  AC_MSG_ERROR(Program "tar" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_CURL, curl, no)
if test x$FIND_CURL = xno; then
  AC_MSG_ERROR(Program "curl" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_MYSQL, mysql, no)
if test x$FIND_MYSQL = xno; then
  AC_MSG_ERROR(Program "mysql" is needed. Install this first.)
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

# Library sqlite3.
PKG_CHECK_MODULES(SQLITE, sqlite3 >= 3.0.0,,AC_MSG_ERROR(sqlite development version >= 3.0.0 is needed.))
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)

# Check for availability of libxml2
AM_PATH_XML2(2.5.0, , AC_MSG_ERROR(Library libxml2-dev is needed. Install this first.))

PKG_CHECK_MODULES(ENCHANT, enchant >= 1.2.0)
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

PKG_CHECK_MODULES(GTKHTML38, libgtkhtml-3.8 >= 3.8.0,
	[
	],
	[
		PKG_CHECK_MODULES(GTKHTML314, libgtkhtml-3.14 >= 3.8.0,
			[
			],
			[
				AC_MSG_ERROR(libgtkhtml3 development version >= 3.8.0 is needed.)
			]
		)
	]
)
AC_SUBST(GTKHTML38_CFLAGS)
AC_SUBST(GTKHTML38_LIBS)
AC_SUBST(GTKHTML314_CFLAGS)
AC_SUBST(GTKHTML314_LIBS)

PKG_CHECK_MODULES(GTKSOURCEVIEW, gtksourceview-2.0 >= 2.2.0)
AC_SUBST(GTKSOURCEVIEW_CFLAGS)
AC_SUBST(GTKSOURCEVIEW_LIBS)

PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.0.0,,AC_MSG_ERROR(libwebkit development version >= 1.0.0 is needed.))
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)

PKG_CHECK_MODULES(DBUSGLIB, dbus-glib-1 >= 0.7.0,,AC_MSG_ERROR(libdbus-glib-1 development version >= 0.7.0 is needed.))
AC_SUBST(DBUSGLIB_CFLAGS)
AC_SUBST(DBUSGLIB_LIBS)

PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4 >= 2.4,,AC_MSG_ERROR(libsoup-2.4 development version >= 2.4 is needed.))
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)


# Checks for header files.
AC_CHECK_HEADER(sqlite3.h, , AC_MSG_ERROR([Header file sqlite3.h was not found. Usually this is in libsqlite3-dev.]))

# This code is kept to learn from.
# AC_CHECK_HEADER(aspell.h, [AC_DEFINE(HAVE_ASPELL_H,1, 
#    [Define to 1 if you have the <aspell.h> header file.] )] )
# AC_CHECK_HEADER(pspell/pspell.h, [AC_DEFINE(HAVE_PSPELL_H,1, 
#    [Define to 1 if you have the <pspell/pspell.h> header file.] )] )
# AC_SUBST(SPELLER_LIB)


# Check for web server on localhost.
AC_MSG_CHECKING(local web server)
curl -s http://localhost > /dev/null
if test $? -ne 0
  then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR(A local web server such as Apache is required but it was not found or it is not running)
    exit 1
fi
AC_MSG_RESULT(yes)


# MySQL database root password.
MYSQL_ROOT_PASSWORD_DEFAULT="root"
AC_ARG_WITH(mysql-root-password,
        [  --with-mysql-root-password=password
                          MySQL database root password],
        [ MYSQL_ROOT_PASSWORD=$withval ],
        [ MYSQL_ROOT_PASSWORD=$MYSQL_ROOT_PASSWORD_DEFAULT ]
)
AC_DEFINE([MYSQL_ROOT_PASSWORD], [], [Define the root password of the MySQL database])
AC_DEFINE_UNQUOTED(MYSQL_ROOT_PASSWORD, "$MYSQL_ROOT_PASSWORD")
AC_SUBST(MYSQL_ROOT_PASSWORD)


# Check access to local MySQL installation
AC_MSG_CHECKING(access to local MySQL server)
echo quit | mysql --user=root --pass=${MYSQL_ROOT_PASSWORD}
if test $? -ne 0
  then
    AC_MSG_RESULT(no)
    AC_MSG_WARN(The local MySQL server could not be accessed)
    AC_MSG_WARN(Is the MySQL server running?)
    AC_MSG_WARN(Is the password for the 'root' user correct?)
    AC_MSG_WARN(Try ./configure --with-mysql-root-password=mysecret)
    AC_MSG_ERROR(Fatal error)
    exit 1
fi
AC_MSG_RESULT(yes)


# Location of web root to be automatically looked up if the user didn't give it.
# Local web server document root.
WEBROOT_DEFAULT="/"
AC_ARG_WITH(web-document-root,
        [  --with-web-document-root=root
                          Local web server document root],
        [ WEBROOT=$withval ],
        [ WEBROOT=$WEBROOT_DEFAULT ]
)
AC_MSG_CHECKING(local web server document root)
if [ [ $WEBROOT == "/" ] ]
  then
    test -d /var/www
    if [ [ $? -eq 0 ] ]
    then
      WEBROOT=/var/www
    fi
    test -d /var/www/html
    if [ [ $? -eq 0 ] ]
    then
      WEBROOT=/var/www/html
    fi
    test -d /srv/www/htdocs
    if [ [ $? -eq 0 ] ]
    then
      WEBROOT=/srv/www/htdocs
    fi
    test -d /Library/WebServer/Documents
    if [ [ $? -eq 0 ] ]
    then
      WEBROOT=/Library/WebServer/Documents
    fi
fi
if [ [ $WEBROOT == "/" ] ]
  then
    AC_MSG_RESULT(no)
    AC_ERROR(Could not find local web server document root folder. Provide the information through: --with-web-document-root=<root>)
fi
AC_MSG_RESULT(${WEBROOT})
AC_DEFINE([WEBROOT], [], [Define the local web server document root])
AC_DEFINE_UNQUOTED(WEBROOT, "$WEBROOT")
AC_SUBST(WEBROOT)


# Web site administrator user name and password.
SITE_ADMIN_USERNAME_DEFAULT="adminusername"
AC_ARG_WITH(site-admin-username,
        [  --with-site-admin-username=username
                          Site Administrator username],
        [ SITE_ADMIN_USERNAME=$withval ],
        [ SITE_ADMIN_USERNAME=$SITE_ADMIN_USERNAME_DEFAULT ]
)
AC_DEFINE([SITE_ADMIN_USERNAME], [], [Define the username of the site administrator])
AC_DEFINE_UNQUOTED(SITE_ADMIN_USERNAME, "$SITE_ADMIN_USERNAME")
AC_SUBST(SITE_ADMIN_USERNAME)
SITE_ADMIN_PASSWORD_DEFAULT="adminpassword"
AC_ARG_WITH(site-admin-password,
        [  --with-site-admin-password=password
                          Site Administrator password],
        [ SITE_ADMIN_PASSWORD=$withval ],
        [ SITE_ADMIN_PASSWORD=$SITE_ADMIN_PASSWORD_DEFAULT ]
)
AC_DEFINE([SITE_ADMIN_PASSWORD], [], [Define the password of the site administrator])
AC_DEFINE_UNQUOTED(SITE_ADMIN_PASSWORD, "$SITE_ADMIN_PASSWORD")
AC_SUBST(SITE_ADMIN_PASSWORD)


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
AC_CONFIG_FILES([Makefile 
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
		  shutdown/Makefile
		  usfm/Makefile
		  kjv/Makefile
		  outpost/Makefile
		  gobible/Makefile
		  web/Makefile
		  dbus/Makefile
		  vcs/Makefile
		  xetex/Makefile
		])
AC_OUTPUT

# Create the file with the database credentials for the web installation.
CREDENTIALS=web/database/credentials.php
rm $CREDENTIALS
echo '<?php'                                                       >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo // Credentials for accessing the MySQL database.              >> $CREDENTIALS
echo // You can modify these to suit your installation.            >> $CREDENTIALS
echo // Be sure to only change the text within the double quotes.  >> $CREDENTIALS
echo // The double quotes themselves should remain in place.       >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo // Host: This is the server where the database runs.          >> $CREDENTIALS
echo '$database_host = "localhost";'                               >> $CREDENTIALS
echo // Name: This is the name of the database.                    >> $CREDENTIALS
echo '$database_name = "bibledit";'                                >> $CREDENTIALS
echo // User: This is the username of the database administrator.  >> $CREDENTIALS
echo '$database_user = "root";'                                    >> $CREDENTIALS
echo // Pass: This is the password of the database administrator.  >> $CREDENTIALS
echo '$database_pass = "'$MYSQL_ROOT_PASSWORD'";'                  >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo '?>'                                                          >> $CREDENTIALS

# Create the file with the login credentials for the site administrator.
CREDENTIALS=web/admin/credentials.php
rm $CREDENTIALS
echo '<?php'                                                       >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo // Login credentials for the site administrator.              >> $CREDENTIALS
echo // You can modify these to suit your installation.            >> $CREDENTIALS
echo // Be sure to only change the text within the double quotes.  >> $CREDENTIALS
echo // The double quotes themselves should remain in place.       >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo // Username.                                                  >> $CREDENTIALS
echo '$site_admin_username = "'$SITE_ADMIN_USERNAME'";'            >> $CREDENTIALS
echo // Password.                                                  >> $CREDENTIALS
echo '$site_admin_password = "'$SITE_ADMIN_PASSWORD'";'            >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo '?>'                                                          >> $CREDENTIALS

# Create the file with the version information.
VERSIONFILE=web/version.php
rm $VERSIONFILE
echo '<?php'                            >> $VERSIONFILE
echo '$version = "'$PACKAGE_VERSION'";' >> $VERSIONFILE
echo '?>'                               >> $VERSIONFILE

# Summary.
echo "
Bibledit $VERSION configuration summary:

        Version                               : ${PACKAGE_VERSION}
        Installation prefix                   : ${prefix}
        Web server document root              : ${WEBROOT}
        MySQL database host                   : localhost
        MySQL database name                   : bibledit
        MySQL database administrator username : root
        MySQL database administrator password : ${MYSQL_ROOT_PASSWORD}
        Website administrator username        : ${SITE_ADMIN_USERNAME}
        Website administrator password        : ${SITE_ADMIN_PASSWORD}
"
