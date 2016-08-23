#!/bin/bash


# This script runs on Linux or macOS.
# It populates a folder with files for Visual Studio.
# Visual Studio can then build Bibledit for Windows.


# Work in a temporal directory separate from the source tree.
SCRIPTDIR=`dirname $0`
cd $SCRIPTDIR
TEMPDIR=/tmp/bibledit-visualstudio
mkdir -p $TEMPDIR


echo Synchronizing relevant source code to the temporal directory.
rsync --archive --exclude '.DS_Store' . $TEMPDIR/
rsync --archive --exclude '.deps' --exclude '*.o' --exclude '*.a' --exclude '.dirstamp' --exclude 'server' --exclude 'unittest' --exclude '.DS_Store' --exclude 'autom4te.cache' --exclude 'bibledit' --exclude '*~' --exclude 'dev' --exclude 'generate' --exclude 'valgrind' ../../lib/* $TEMPDIR/server


cd $TEMPDIR/server


# Configure the Bibledit source.
# It is configured in the local directory rather than in the Windows share,
# because it fails to write in the Windows share.
./configure --enable-windows --with-network-port=9876 --enable-visualstudio
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi
# Remove some Linux related definitions as they don't work on Windows.
sed -i.bak '/HAVE_LIBPROC/d' config.h
sed -i.bak '/HAVE_EXECINFO/d' config.h


# Remove the .deps folders.
find . -name ".deps" -type d -prune -exec rm -rf '{}' '+'


# Synchronize the configured source to the Windows share.
# This enabled Visual Studio on Windows to work in a local folder rather than in a share, as that does not work reliably.
echo Synchronizing source to the Windows share.
rsync --archive $TEMPDIR /Volumes/visualstudio/Projects


# Fix g++.exe: error: unrecognized command line option '-rdynamic'
# Fix undefined reference to `_imp__*' by adding required library to the linker.
# sed -i 's/-rdynamic/-lws2_32/' Makefile.am
