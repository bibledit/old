#!/bin/bash


# Synchronize and build libbibledit on OS X for OS X.


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
sed -i.bak 's#\`xml2-config --cflags\`#-I/usr/include/libxml2#g' Makefile
sed -i.bak 's#-pedantic#-isysroot\ '$SDK'#g' Makefile
sed -i.bak 's#/opt/local/include#. -I..#g' Makefile
sed -i.bak 's#\`xml2-config --libs\`#-lxml2 -lz -lpthread -liconv -lm#g' Makefile
sed -i.bak 's#/opt/local/lib#.#g' Makefile
sed -i.bak 's#-L.#-L. -L../lib#g' Makefile


# Update the configuration.
sed -i.bak '/CONFIG_ENABLE_FILE_UPLOAD/d' config/config.h
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


popd


say Building for OS X is ready
