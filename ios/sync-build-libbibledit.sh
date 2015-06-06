#!/bin/bash


# Synchronize and build libbibledit on OS X for iOS.


# Clean the Bibledit library.
function clean
{
  pushd webroot
  echo Clean source.
  make clean > /dev/null
  popd
}


# Build libbibledit for one iOS architecure.
# This script runs on OS X.
function compile
{

ARCH=$1
PLATFORM=$2
BITS=$3
echo Compile for architecture $ARCH $BITS bits

export IPHONEOS_DEPLOYMENT_TARGET="6.0"
SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/$PLATFORM.platform/Developer/SDKs/$PLATFORM.sdk
TOOLDIR=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
COMPILEFLAGS="-Wall -Wextra -pedantic -g -O2 -c"

CURLINCLUDE=../dependencies/libcurl/ios/include/curl

pushd $CURLINCLUDE
cp curlbuild$BITS.h curlbuild.h
popd > /dev/null

pushd webroot

CPPFILES=(

library/bibledit.cpp
webserver/webserver.cpp
webserver/http.cpp

)

CFILES=(

webserver/io.c
json/json.c
library/locks.c

)

for cpp in ${CPPFILES[@]}; do

extension="${cpp##*.}"
basepath="${cpp%.*}"
echo Compiling $cpp

$TOOLDIR/clang++ -arch ${ARCH} -isysroot $SYSROOT -I. -I/usr/include/libxml2 -I../dependencies/libcurl/ios/include $COMPILEFLAGS -std=c++11 -stdlib=libc++ -o $basepath.o $cpp
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

done

for c in ${CFILES[@]}; do

extension="${c##*.}"
basepath="${c%.*}"
echo Compiling $c

$TOOLDIR/clang -arch ${ARCH} -isysroot $SYSROOT -I. $COMPILEFLAGS -o $basepath.o $c
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

done

popd

pushd $CURLINCLUDE
rm curlbuild.h
popd


# Linking
echo Linking

pushd webroot

$TOOLDIR/ar cru libbibledit.a `find . -name *.o`
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

$TOOLDIR/ranlib libbibledit.a
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

popd


# Copy output to desktop

pushd webroot
cp libbibledit.a ~/Desktop/libbibledit-$ARCH.a
rm libbibledit.a
popd

}


# Sychronizes the libbibledit data files in the source tree to iOS and cleans them up.
rsync -av --delete ../lib/ webroot
pushd webroot
./configure
make distclean
rm -f bibledit
rm -r autom4te.cache
rm dev
rm reconfigure
rm -f server
rm -f unittest
rm valgrind
rm -r xcode
rm -r executable
popd

# Configure Bibledit in client mode,
# Run only only one parallel task so the interface is more responsive.
# Enable the single-tab browser.
pushd webroot
./configure --enable-client --with-parallel-tasks=1 --enable-bare-browser
popd

clean
compile armv7 iPhoneOS 32

clean
compile armv7s iPhoneOS 32

clean
compile arm64 iPhoneOS 64

clean
compile i386 iPhoneSimulator 32

clean
compile x86_64 iPhoneSimulator 64

cp webroot/library/bibledit.h include

echo Creating fat library file
lipo -create -output ~/Desktop/libbibledit.a ~/Desktop/libbibledit-armv7.a ~/Desktop/libbibledit-armv7s.a ~/Desktop/libbibledit-arm64.a ~/Desktop/libbibledit-i386.a ~/Desktop/libbibledit-x86_64.a
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi
lipo -info ~/Desktop/libbibledit.a
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

echo Copying library into place
mv ~/Desktop/libbibledit.a lib

echo Clean libraries from desktop
rm ~/Desktop/libbibledit-armv7.a
rm ~/Desktop/libbibledit-armv7s.a
rm ~/Desktop/libbibledit-arm64.a
rm ~/Desktop/libbibledit-i386.a
rm ~/Desktop/libbibledit-x86_64.a

echo Clean webroot
pushd webroot
rm aclocal.m4
rm AUTHORS
rm ChangeLog
rm compile
rm config.guess
rm config.h.in
rm config.log
rm config.status
rm config.sub
rm configure
rm configure.ac
rm COPYING
rm depcomp
rm DEVELOP
rm INSTALL
rm install-sh
rm Makefile
rm Makefile.in
rm Makefile.am
rm missing
rm NEWS
rm README
rm stamp-h1
popd

say Compile for iOS is ready


