#!/bin/bash

LOG="autogen.log"
unamestr=`uname`

function error_exit
{
    echo "Something went wrong with $1; check $LOG" 1>&2
    exit 1
}

mkdir -p m4

echo "---------------------------------------------------------" >> $LOG
echo "Running Bibledit autogen.sh" >> $LOG
date >> $LOG

echo "Running autoreconf..."
echo "autoreconf---------------------------------------" >> $LOG
autoreconf --force --install >> $LOG 2>&1

echo "Creating m4/aclocal.m4..."
test -r m4/aclocal.m4 || touch m4/aclocal.m4

echo "Copying some autoconf macros on system $unamestr..."
echo "Copying macros-----------------------------------" >> $LOG
# First case is msys2 on Windows
if [ -n "$MSYSTEM" ]
then
  case "$MSYSTEM" in
    MINGW32)
	  ACLOCAL="/mingw32/share/aclocal"
    ;;
    MINGW64)
	  ACLOCAL="/mingw64/share/aclocal"
    ;;
    MSYS)
      error_exit "copying autoconf macros"
    ;;
    *)
      echo "Unknown value $MSYSTEM in MSYSTEM environment variable, so I don't know what to do."
      exit 2
    ;;
  esac
# Second case I can handle is on Linux
elif [ $unamestr == "Linux" ]
then
  ACLOCAL="/usr/share/aclocal"
else
  error_exit "figuring out what system I am on, so I can copy autoconf macros"
fi

cp -f "$ACLOCAL/codeset.m4" m4/
cp -f "$ACLOCAL/gettext.m4" m4/
cp -f "$ACLOCAL/glib-gettext.m4" m4/
cp -f "$ACLOCAL/glibc21.m4" m4/
cp -f "$ACLOCAL/iconv.m4" m4/
cp -f "$ACLOCAL/lcmessage.m4" m4/
cp -f "$ACLOCAL/progtest.m4" m4/
# What to do about isc-posix.m4?

echo "Running glib-gettextize..."
echo "glib-gettextize----------------------------------" >> $LOG
echo "Ignore non-fatal messages" >> $LOG
echo "no" | glib-gettextize --force --copy >> $LOG 2>&1

echo "Making m4/aclocal.m4 writable ..."
echo "Making m4/aclocal.m4 writable--------------------" >> $LOG
test -r m4/aclocal.m4 && chmod u+w m4/aclocal.m4

echo "Running intltoolize..."
echo "intltoolize--------------------------------------" >> $LOG
intltoolize --force --copy --automake  >> $LOG 2>&1 || error_exit "intltoolize"

echo "Running aclocal..."
echo "aclocal------------------------------------------" >> $LOG
#-I /mingw/msys/1.0/share/aclocal
aclocal -I m4 --install  >> $LOG 2>&1 || error_exit "aclocal"

echo "Running libtoolize..."
echo "libtoolize---------------------------------------" >> $LOG
libtoolize  >> $LOG 2>&1 || error_exit "libtoolize"

echo "Running autoheader..."
echo "autoheader---------------------------------------" >> $LOG
autoheader  >> $LOG 2>&1 || error_exit "autoheader"

echo "Running autoconf..."
echo "autoconf-----------------------------------------" >> $LOG
autoconf  >> $LOG 2>&1 || error_exit "autoconf"

echo "Running automake..."
echo "automake-----------------------------------------" >> $LOG
automake --add-missing  >> $LOG 2>&1 || error_exit "automake"

echo "You should now run ./configure, then make."
if [ $unamestr == "Linux" ]
then
  echo "After that, you should run sudo make install"
elif [ $unamestr == "MINGW32_NT-6.1" ]
then
  echo "After that, you should run windows/installWin.sh"
elif [ $unamestr == "MINGW64_NT-6.1" ]
then
  echo "After that, you should run windows/installWin.sh"
else
  echo "After that, I'm not quite sure how to install on your system."
fi
echo "Check $LOG if you want to see some details about what we just did."
