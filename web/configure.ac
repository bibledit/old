#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)
AC_INIT([bibledit-web],[0.5.54],[http://www.nongnu.org/bibledit])
ac_default_prefix=/usr
AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_HEADERS([config.php])

# Checks for programs.

AC_PATH_PROG(MAKE_PATH, make, no)
if test x$MAKE_PATH = xno; then
  AC_MSG_ERROR(Program "make" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_GIT, git, no)
if test x$FIND_GIT = xno; then
  AC_MSG_ERROR(Program "git" (usually called git-core) is needed. Install this first.)
fi

AC_PATH_PROG(FIND_MYSQL, mysql, no)
if test x$FIND_MYSQL = xno; then
  AC_MSG_ERROR(Program "mysql" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_MYSQLDUMP, mysqldump, no)
if test x$FIND_MYSQLDUMP = xno; then
  AC_MSG_ERROR(Program "mysqldump" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_PHP, php, no)
if test x$FIND_PHP = xno; then
  AC_MSG_ERROR(Program "php" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_JAVA, java, no)
if test x$FIND_JAVA = xno; then
  AC_MSG_ERROR(Program "java" is needed. Install the Java Runtime Environment.)
fi

AC_PATH_PROG(FIND_ZIP, zip, no)
if test x$FIND_ZIP = xno; then
  AC_MSG_ERROR(Program "zip" is needed. Install this first.)
fi

AC_PATH_PROG(FIND_UNZIP, unzip, no)
if test x$FIND_UNZIP = xno; then
  AC_MSG_ERROR(Program "unzip" is needed. Install this first.)
fi

# AC_PATH_PROG(FIND_JAVAC, javac, no)
# if test x$FIND_JAVAC = xno; then
#   AC_MSG_ERROR(Program "javac" is needed. Install the Java Development Kit.)
# fi

# Check for web server on localhost.
# AC_MSG_CHECKING(local web server)
# curl -s http://localhost > /dev/null
# if test $? -ne 0
#   then
#     AC_MSG_RESULT(no)
#     AC_MSG_ERROR(A local web server such as Apache is required but it was not found or it is not running)
#     exit 1
# fi
# AC_MSG_RESULT(yes)


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
# AC_MSG_CHECKING(access to local MySQL server)
# echo quit | mysql --user=root --pass=${MYSQL_ROOT_PASSWORD}
# if test $? -ne 0
#   then
#     AC_MSG_RESULT(no)
#     AC_MSG_WARN(The local MySQL server could not be accessed)
#     AC_MSG_WARN(Is the MySQL server running?)
#     AC_MSG_WARN(Is the password for the 'root' user correct?)
#     AC_MSG_WARN(Try ./configure --with-mysql-root-password=mysecret)
#     AC_MSG_ERROR(Fatal error)
#     exit 1
# fi
# AC_MSG_RESULT(yes)


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
    AC_MSG_ERROR(Could not find local web server document root folder. Provide the information through: --with-web-document-root=<root>)
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


# Location of xercesImpl.jar.
# Obtain it through dpkg -L libxerces2-java
# XERCESPATH_DEFAULT="."
# AC_ARG_WITH(xerces-path,
#         [  --with-xerces-path=path
#                           xercesImpl.jar path],
#         [ XERCESPATH=$withval ],
#         [ XERCESPATH=$XERCESPATH_DEFAULT ]
# )
# AC_MSG_CHECKING(xercesImpl.jar path)
# if [ [ $XERCESPATH == "." ] ]
#   then
#     test -f /usr/share/java/xercesImpl.jar
#     if [ [ $? -eq 0 ] ]
#     then
#       XERCESPATH=/usr/share/java/xercesImpl.jar
#     fi
# fi
# if [ [ $XERCESPATH == "." ] ]
#   then
#     AC_MSG_RESULT(no)
#     AC_MSG_ERROR(Could not find path to xercesImpl.jar. Provide the information through: --with-xerces-path=<path>)
# fi
# AC_MSG_RESULT(${XERCESPATH})
# AC_DEFINE([XERCESPATH], [], [Define the path to xercesImpl.jar])
# AC_DEFINE_UNQUOTED(XERCESPATH, "$XERCESPATH")
# AC_SUBST(XERCESPATH)


# Location of the Zend Framework.
# Obtain it through dpkg -L libzend-framework-php on Debian and derivatives.
ZENDPATH_DEFAULT="."
AC_ARG_WITH(zend-path,
        [  --with-zend-path=path   Zend Framework path],
        [ ZENDPATH=$withval ],
        [ ZENDPATH=$ZENDPATH_DEFAULT ]
)
AC_MSG_CHECKING(Zend Framework path)
if [ [ $ZENDPATH == "." ] ]
  then
    test -d /usr/share/php/libzend-framework-php
    if [ [ $? -eq 0 ] ]
    then
      ZENDPATH=/usr/share/php/libzend-framework-php
    fi
fi
if [ [ $ZENDPATH == "." ] ]
  then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR(Could not find path to the Zend Framework. Provide the information through: --with-zend-path=<path>)
fi
AC_MSG_RESULT(${ZENDPATH})
AC_DEFINE([ZENDPATH], [], [Define the path to zendImpl.jar])
AC_DEFINE_UNQUOTED(ZENDPATH, "$ZENDPATH")
AC_SUBST(ZENDPATH)


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
		  web/Makefile
		  tests/Makefile
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
CREDENTIALS=web/administration/credentials.php
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

# Create the file with the paths to the system libraries.
PATHSFILE=web/paths/paths.php
rm $PATHSFILE
echo '<?php'                            >> $PATHSFILE
# echo '$xercesPath = "'$XERCESPATH'";'   >> $PATHSFILE
echo '$zendPath = "'$ZENDPATH'";'       >> $PATHSFILE
echo '?>'                               >> $PATHSFILE

# Summary.
echo "
Bibledit-Web $VERSION configuration summary:

Version                               : ${PACKAGE_VERSION}
Installation prefix                   : ${prefix}
Web server document root              : ${WEBROOT}
MySQL database host                   : localhost
MySQL database name                   : bibledit
MySQL database administrator username : root
MySQL database administrator password : ${MYSQL_ROOT_PASSWORD}
Website administrator username        : ${SITE_ADMIN_USERNAME}
Website administrator password        : ${SITE_ADMIN_PASSWORD}
Zend Framework path                   : ${ZENDPATH}
"

