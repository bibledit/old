#!/bin/bash


# This script runs from within a Cygwin shell.
# It refreshes the bibledit library sources.
# It builds the bibledit library for Windows.


pushd webroot


rsync -av --delete ../../lib/ .
make distclean


./configure --enable-client --enable-urlsettings


make


mkdir -p /cygdrive/c/bibledit 
cp -r * /cygdrive/c/bibledit


popd
pushd /cygdrive/c/bibledit


find . -name "*.h" -delete
find . -name "*.cpp" -delete
find . -name "*.c" -delete
find . -name "*.o" -delete
rm *.m4
rm -r autom*cache
rm bibledit
rm compile
rm config.*
rm configure*
rm depcomp
rm dev
rm install-sh
rm missing
rm reconfigure
rm valgrind
find . -name ".deps" -exec rm -r "{}" \; > /dev/null 2>&1
find . -name ".dirstamp" -delete
rm Makefile*
rm stamp-h1
rm unittest.exe
rm libbibledit.a
rm -rf xcode
rm DEVELOP
rm INSTALL


cp /usr/bin/cyg*.dll .


popd
