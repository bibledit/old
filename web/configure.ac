#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ(2.64)
AC_INIT([bibledit-web],[1.01],[http://bibledit.org])
AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
ac_default_prefix=/usr
AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_HEADERS([config.php])


AC_PATH_PROG(MAKE_PATH, make, no)
if test x$MAKE_PATH = xno; then
  AC_MSG_ERROR(Program "make" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_GIT, git, no)
if test x$FIND_GIT = xno; then
  AC_MSG_ERROR(Program "git" is needed. Install this first.)
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


AC_PATH_PROG(FIND_ZIP, zip, no)
if test x$FIND_ZIP = xno; then
  AC_MSG_ERROR(Program "zip" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_UNZIP, unzip, no)
if test x$FIND_UNZIP = xno; then
  AC_MSG_ERROR(Program "unzip" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_WDIFF, wdiff, no)
if test x$FIND_WDIFF = xno; then
  AC_MSG_ERROR(Program "wdiff" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_SQLITE3, sqlite3, no)
if test x$FIND_SQLITE3 = xno; then
  AC_MSG_ERROR(Program "sqlite3" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_PYTHON, python, no)
if test x$FIND_PYTHON = xno; then
  AC_MSG_ERROR(Program "python" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_PERL, perl, no)
if test x$FIND_PERL = xno; then
  AC_MSG_ERROR(Program "perl" is needed. Install this first.)
fi


AC_PATH_PROG(FIND_OSIS2MOD, osis2mod, no)
if test x$FIND_OSIS2MOD = xno; then
  AC_MSG_ERROR(Program "osis2mod" is needed. Install this first.)
fi


# Check for PHP Internationalization support.
AC_MSG_CHECKING(PHP Internationalization support)
php -i | grep intl > /dev/null
if test $? -ne 0
  then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR(PHP module intl not found. Install package php5-intl)
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


# Location of the Zend framework.
# Debian: dpkg -L zendframework: /usr/share/php/Zend
# Ubuntu: dpkg -L libzend-framework-php: /usr/share/php/libzend-framework-php/Zend
ZENDPATH_DEFAULT="."
AC_ARG_WITH(zend-path,
        [  --with-zend-path=path   Zend Framework path],
        [ ZENDPATH=$withval ],
        [ ZENDPATH=$ZENDPATH_DEFAULT ]
)
AC_MSG_CHECKING(zend framework path)
if [ [ $ZENDPATH == "." ] ]
then
  test -d /usr/share/php/Zend
  if [ [ $? -eq 0 ] ]
  then
    ZENDPATH=/usr/share/php/Zend
  fi
fi
if [ [ $ZENDPATH == "." ] ]
then
  test -d /usr/share/php/libzend-framework-php/Zend
  if [ [ $? -eq 0 ] ]
  then
    ZENDPATH=/usr/share/php/libzend-framework-php/Zend
  fi
fi
if [ [ $ZENDPATH == "." ] ]
then
  AC_MSG_RESULT(no)
  AC_MSG_ERROR(Could not find path to the zend framework. Provide the information through: --with-zend-path=<path>)
fi
AC_MSG_RESULT(${ZENDPATH})
AC_DEFINE([ZENDPATH], [], [Define the path to the zend framework])
AC_DEFINE_UNQUOTED(ZENDPATH, "$ZENDPATH")
AC_SUBST(ZENDPATH)


# Location of jQuery.
# Ubuntu: dpkg -L libjs-jquery: /usr/share/javascript/jquery/jquery.js
# JQUERYPATH_DEFAULT="."
# AC_ARG_WITH(jquery-path,
#         [  --with-jquery-path=path   jQuery path],
#         [ JQUERYPATH=$withval ],
#         [ JQUERYPATH=$JQUERYPATH_DEFAULT ]
# )
# AC_MSG_CHECKING(jquery path)
# if [ [ $JQUERYPATH == "." ] ]
# then
#   test -d /usr/share/javascript/jquery
#   if [ [ $? -eq 0 ] ]
#   then
#     JQUERYPATH=/usr/share/javascript/jquery
#   fi
# fi
# if [ [ $JQUERYPATH == "." ] ]
# then
#   test -d /usr/share/javascript/jquery
#   if [ [ $? -eq 0 ] ]
#   then
#     JQUERYPATH=/usr/share/javascript/jquery
#   fi
# fi
# if [ [ $JQUERYPATH == "." ] ]
# then
#   AC_MSG_RESULT(no)
#   AC_MSG_ERROR(Could not find path to jQuery. Provide the information through: --with-jquery-path=<path>)
# fi
# AC_MSG_RESULT(${JQUERYPATH})
# AC_DEFINE([JQUERYPATH], [], [Define the path to jquery])
# AC_DEFINE_UNQUOTED(JQUERYPATH, "$JQUERYPATH")
# AC_SUBST(JQUERYPATH)


# Location of jQuery UI.
# Ubuntu: dpkg -L libjs-jquery-ui: /usr/share/javascript/jquery-ui/...
# JQUERYUIPATH_DEFAULT="."
# AC_ARG_WITH(jqueryui-path,
#         [ --with-jqueryui-path=path   jQuery UI path],
#         [ JQUERYUIPATH=$withval ],
#         [ JQUERYUIPATH=$JQUERYUIPATH_DEFAULT ]
# )
# AC_MSG_CHECKING(jqueryui path)
# if [ [ $JQUERYUIPATH == "." ] ]
# then
#   test -d /usr/share/javascript/jquery-ui
#   if [ [ $? -eq 0 ] ]
#   then
#     JQUERYUIPATH=/usr/share/javascript/jquery-ui
#   fi
# fi
# if [ [ $JQUERYUIPATH == "." ] ]
# then
#   test -d /usr/share/javascript/jquery-ui
#   if [ [ $? -eq 0 ] ]
#   then
#     JQUERYUIPATH=/usr/share/javascript/jquery-ui
#   fi
# fi
# if [ [ $JQUERYUIPATH == "." ] ]
# then
#   AC_MSG_RESULT(no)
#   AC_MSG_ERROR(Could not find path to jQuery UI. Provide the information through: --with-jqueryui-path=<path>)
# fi
# AC_MSG_RESULT(${JQUERYUIPATH})
# AC_DEFINE([JQUERYUIPATH], [], [Define the path to jquery-ui])
# AC_DEFINE_UNQUOTED(JQUERYUIPATH, "$JQUERYUIPATH")
# AC_SUBST(JQUERYUIPATH)


# Location on the web and internal.
LOCATION_DEFAULT="bibledit-web"
AC_ARG_WITH(location,
        [  --with-location=location  Location on the web and internal],
        [ LOCATION=$withval ],
        [ LOCATION=$LOCATION_DEFAULT ]
)
AC_DEFINE([LOCATION], [], [Define the location on the web and internal])
AC_DEFINE_UNQUOTED(LOCATION, "$LOCATION")
AC_SUBST(LOCATION)


# Whether to have the default password protected installation, or an open installation.
OPEN_INSTALLATION_DEFAULT="false"
AC_ARG_WITH(open-installation,
        [  --with-open-installation=true
                          Open installation],
        [ OPEN_INSTALLATION=$withval ],
        [ OPEN_INSTALLATION=$OPEN_INSTALLATION_DEFAULT ]
)
AC_DEFINE([OPEN_INSTALLATION], [], [Whether to have an open installation])
AC_DEFINE_UNQUOTED(OPEN_INSTALLATION, "$OPEN_INSTALLATION")
AC_SUBST(OPEN_INSTALLATION)


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


# Set directories
packagedatadir=share/${PACKAGE}
AC_SUBST(packagedatadir)

packagelocalstatedir=var/lib/${PACKAGE}
AC_SUBST(packagelocalstatedir)

systemconfigdir=etc/${PACKAGE}
AC_SUBST(systemconfigdir)


# Create all the Makefiles.
AC_CONFIG_FILES([
  Makefile 
  fhs/Makefile
  web/Makefile
  tests/Makefile
  apache/Makefile
  cron/Makefile
  mysql/Makefile
  secure/Makefile
])
AC_OUTPUT


# Create the file with the database credentials.
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
echo '$database_name = "'$LOCATION'";'                             >> $CREDENTIALS
echo // User: This is the username of the database administrator.  >> $CREDENTIALS
echo '$database_user = "root";'                                    >> $CREDENTIALS
echo // Pass: This is the password of the database administrator.  >> $CREDENTIALS
echo '$database_pass = "'$MYSQL_ROOT_PASSWORD'";'                  >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo '?>'                                                          >> $CREDENTIALS


# Create the file with the login credentials for the site administrator,
# and whether this is an open installation, or the default password protected one.
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
echo // Whether this is an open installation.                      >> $CREDENTIALS
echo // false: It is the standard password protected installation. >> $CREDENTIALS
echo // true: It is an open installation.                          >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo // Open installation.                                         >> $CREDENTIALS
echo '$open_installation = '$OPEN_INSTALLATION';'                  >> $CREDENTIALS
echo                                                               >> $CREDENTIALS
echo '?>'                                                          >> $CREDENTIALS


# Create the file with the version information.
VERSIONFILE=web/version.php
rm $VERSIONFILE
echo '<?php'                            >> $VERSIONFILE
echo '$version = "'$PACKAGE_VERSION'";' >> $VERSIONFILE
echo '?>'                               >> $VERSIONFILE


# Create the file with the various system paths.
PATHSFILE=web/paths/paths.php
rm $PATHSFILE
echo '<?php' >> $PATHSFILE
echo '$zendPath = "'$ZENDPATH'";' >> $PATHSFILE
echo '$localStatePath = "'$DESTDIR'/'$packagelocalstatedir'";' >> $PATHSFILE
echo '$location = "'$LOCATION'";' >> $PATHSFILE
echo '?>' >> $PATHSFILE


# Summary.
echo "
Bibledit-Web $VERSION configuration summary:

Installation prefix: ${prefix}
Location on web and internal: ${LOCATION}
Open installation: ${OPEN_INSTALLATION}
MySQL database host: localhost
MySQL database name: ${LOCATION}
MySQL database administrator username: root
MySQL database administrator password: ${MYSQL_ROOT_PASSWORD}
Website administrator username: ${SITE_ADMIN_USERNAME}
Website administrator password: ${SITE_ADMIN_PASSWORD}
Zend framework path: ${ZENDPATH}
Local state directory: $DESTDIR/${packagelocalstatedir}
"

