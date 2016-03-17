#!/bin/bash


# Synchronize and build libbibledit on OS X for OS X.


# Take the relevant source code for building Bibledit for OSX.
# Put it in a temporal location.
# The purpose by doing so is not to have duplicated source code for the bibledit library.
# This approach does not clutter the bibledit git repositoy with the built files.
OSXSOURCE=`dirname $0`
cd $OSXSOURCE
BIBLEDITOSX=/tmp/bibledit-osx
echo Synchronizing relevant source code to $BIBLEDITOSX
mkdir -p $BIBLEDITOSX
rm $BIBLEDITOSX/* 2> /dev/null
rsync --archive --delete ../lib $BIBLEDITOSX/
rsync --archive --delete ../osx $BIBLEDITOSX/


# From now on the working directory is the temporal location.
cd $BIBLEDITOSX/osx


pushd webroot


# Sychronizes the libbibledit data files in the source tree to OS X and cleans them up.
rsync -av --delete ../../lib/ .
./configure
make distclean
rm config.h


# Xcode's toolchain for C and C++.
export CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CXX="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"


# Xcode's OS X SDK.
SDK=`xcrun --show-sdk-path`


# Configure Bibledit in client mode.
./configure --enable-client --enable-bare-browser --with-network-port=9876


# Update the Makefile.
# The -mmmacosx-version-min is for fixing: ld: warning: object file (libbibledit.a(bibledit.o)) was built for newer OSX version (10.11) than being linked (10.10)
# sed -i.bak 's#\`xml2-config --cflags\`#-I/usr/include/libxml2#g' Makefile
sed -i.bak 's#-pedantic#-mmacosx-version-min=10.10\ -isysroot\ '$SDK'#g' Makefile
sed -i.bak 's#/opt/local/include#. -I..#g' Makefile
# sed -i.bak 's#\`xml2-config --libs\`#-lxml2 -lz -lpthread -liconv -lm#g' Makefile
sed -i.bak 's#/opt/local/lib#.#g' Makefile
sed -i.bak 's#-L.#-L. -L../lib#g' Makefile
sed -i.bak '/SWORD_CFLAGS =/d' Makefile
sed -i.bak '/SWORD_LIBS =/d' Makefile


# Update the configuration.
sed -i.bak '/CONFIG_ENABLE_FILE_UPLOAD/d' config/config.h
sed -i.bak '/HAVE_SWORD/d' config.h
rm config/*.bak


# Build the Bibledit library.
make -j `sysctl -n hw.logicalcpu_max`


# Save the header file.
cp library/bibledit.h ../osx


# Clean out stuff no longer needed.
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
rm server
rm -f unittest
rm stamp-h1
rm generate


popd


say Building for OS X is ready


echo To build the app for OSX open the project in Xcode:
echo open $BIBLEDITOSX/osx/Bibledit.xcodeproj
echo Then build it from within Xcode
