#!/bin/sh

function error_exit
{
    echo "Something went wrong with $1" 1>&2
    exit 1
}

mkdir -p m4

echo "Running autoreconf..."
autoreconf --force --install 

echo "Creating m4/aclocal.m4 ..."
test -r m4/aclocal.m4 || touch m4/aclocal.m4

echo "Running glib-gettextize... Ignore non-fatal messages."
echo "no" | glib-gettextize --force --copy

echo "Making m4/aclocal.m4 writable ..."
test -r m4/aclocal.m4 && chmod u+w m4/aclocal.m4

echo "Running intltoolize..."
intltoolize --force --copy --automake || error_exit "intltoolize"

echo "Running aclocal..."
#-I /mingw/msys/1.0/share/aclocal
aclocal -I m4 --install || error_exit "aclocal"

echo "Running libtoolize..."
libtoolize || error_exit "libtoolize"

echo "Running autoheader..."
autoheader || error_exit "autoheader"

echo "Running autoconf..."
autoconf || error_exit "autoconf"

echo "Running automake..."
automake --add-missing || error_exit "automake"

echo "You should now run ./configure, then make, then make install..."
