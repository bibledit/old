#!/bin/bash


echo Synchronize and build libbibledit on macOS for macOS.


echo Take the relevant source code for building Bibledit for macOS.
echo Put it in a temporal location.
echo Purpose: Not to have duplicated source code for the bibledit library.
echo This does not clutter the bibledit git repository with the build files.
MACOSSOURCE=`dirname $0`
cd $MACOSSOURCE
if [ $? != 0 ]; then exit; fi
BIBLEDITMACOS=/tmp/bibledit-macos
echo Synchronizing relevant source code to $BIBLEDITMACOS
mkdir -p $BIBLEDITMACOS
rm $BIBLEDITMACOS/* 2> /dev/null
rsync --archive --delete ../lib $BIBLEDITMACOS/
if [ $? != 0 ]; then exit; fi
rsync --archive --delete ../macos $BIBLEDITMACOS/
if [ $? != 0 ]; then exit; fi


echo From now on the working directory is the temporal location.
cd $BIBLEDITMACOS/macos
if [ $? != 0 ]; then exit; fi


pushd webroot
if [ $? != 0 ]; then exit; fi


echo Sychronize the libbibledit data files in the source tree to macOS.
rsync -a --delete ../../lib/ .
if [ $? != 0 ]; then exit; fi
echo Dist-clean the Bibledit library.
./configure
if [ $? != 0 ]; then exit; fi
make distclean
if [ $? != 0 ]; then exit; fi
rm config.h


echo Export the Xcode toolchain for C and C++.
export CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CXX="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"


echo The Xcode macOS SDK.
SDK=`xcrun --show-sdk-path`


echo Configure Bibledit in client mode.
./configure --enable-mac
if [ $? != 0 ]; then exit; fi
echo 9876 > config/network-port


echo Update the Makefile.
echo The -mmmacosx-version-min is for fixing: ld: warning:
echo object file libbibledit.a was built for newer OSX version than being linked.
# sed -i.bak 's#\`xml2-config --cflags\`#-I/usr/include/libxml2#g' Makefile
sed -i.bak 's#-pedantic#-mmacosx-version-min=10.10\ -isysroot\ '$SDK'#g' Makefile
if [ $? != 0 ]; then exit; fi
sed -i.bak 's#/opt/local/include#. -I..#g' Makefile
if [ $? != 0 ]; then exit; fi
sed -i.bak 's#/opt/local/lib#.#g' Makefile
if [ $? != 0 ]; then exit; fi
sed -i.bak '/SWORD_CFLAGS =/d' Makefile
if [ $? != 0 ]; then exit; fi
sed -i.bak '/SWORD_LIBS =/d' Makefile
if [ $? != 0 ]; then exit; fi
sed -i.bak '/OPENSSL_LIBS =/d' Makefile
if [ $? != 0 ]; then exit; fi


echo Update the configuration.
sed -i.bak '/CONFIG_ENABLE_FILE_UPLOAD/d' config/config.h
if [ $? != 0 ]; then exit; fi
rm config/*.bak


echo Build the Bibledit library.
make -j `sysctl -n hw.logicalcpu_max`
if [ $? != 0 ]; then exit; fi


echo Save the header file.
cp library/bibledit.h ../macos
if [ $? != 0 ]; then exit; fi


echo Clean out stuff no longer needed.
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
rm -rf sources
rm -rf unittests


popd
if [ $? != 0 ]; then exit; fi


echo Build the app.
cd $BIBLEDITMACOS/macos
xcodebuild
if [ $? != 0 ]; then exit; fi


echo To graphically build the app for macOS, open the project in Xcode:
echo open $BIBLEDITMACOS/macos/Bibledit.xcodeproj
echo Then build it from within Xcode
