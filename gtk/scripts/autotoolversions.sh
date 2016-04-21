#!/bin/bash

echo "Versions of various autotools"
autoreconf --version | grep autoreconf
glib-gettextize --version | grep gettextize
intltoolize --version
aclocal --version | grep aclocal
libtoolize --version | grep libtoolize
autoheader --version | grep autoheader
autoconf --version | grep autoconf
automake --version | grep automake
