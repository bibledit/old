#!/bin/bash


# This script runs on Unix.
# It populates a folder with files for Visual Studio.
# Visual Studio can then build Bibledit for Windows.


# Work in a directory separate from the source tree.
SCRIPTDIR=`dirname $0`
cd $SCRIPTDIR
WORKDIR=~/dev/visualstudio


ls $WORKDIR

exit





echo Synchronizing relevant source code to the temporal directory.
mkdir -p $TEMPDIR
rsync --archive --delete --exclude '.deps' --exclude '*.o' --exclude '*.a' --exclude '.dirstamp' --exclude 'server' --exclude 'unittest' --exclude '.DS_Store' ../lib $TEMPDIR/
# rsync --archive --delete --exclude '.DS_Store'../mingw $TEMPDIR/


cd $TEMPDIR/lib


# Fix g++.exe: error: unrecognized command line option '-rdynamic'
# Fix undefined reference to `_imp__*' by adding required library to the linker.
sed -i 's/-rdynamic/-lws2_32/' Makefile.am


# Limit the maximum number of parallel background tasks 
# to make the interface more responsive.
./configure --enable-windows --with-network-port=9876 --enable-mingw
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi


# Build the library.
make
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi


exit


# Remove some data that is not needed.
rm -rf sources
rm -rf unittests


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
