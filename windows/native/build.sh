#!/bin/bash


# Build Bibledit for Windows.
# Run this script from within a Mingw shell.


# Work in a temporal directory separate from the source tree.
SCRIPTDIR=`dirname $0`
cd $SCRIPTDIR
TEMPDIR=/tmp/bibledit-windows


echo Synchronizing relevant source code to the temporal directory.
mkdir -p $TEMPDIR
rsync --archive --delete --exclude '.deps' --exclude '*.o' --exclude '*.a' --exclude '.dirstamp' --exclude 'server' --exclude 'unittest' --exclude '.DS_Store' ../../lib $TEMPDIR/
# rsync --archive --delete --exclude '.DS_Store'../native $TEMPDIR/


cd $TEMPDIR/lib


# Fix g++.exe: error: unrecognized command line option '-rdynamic'
sed -i 's/-rdynamic//' Makefile.am



# Limit the maximum number of parallel background tasks 
# to make the interface more responsive.
./configure --enable-windows --with-network-port=9876 --enable-msys
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
