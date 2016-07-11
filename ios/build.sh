# Synchronize and build libbibledit on OS X for iOS.


# Clean the Bibledit library.
function clean
{
  pushd webroot
  echo Clean source.
  make clean > /dev/null
  find . -name "*.o" -delete
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
COMPILEFLAGS="-Wall -Wextra -pedantic -g -O2 -c -I.."

pushd webroot

# The following command saves all source files from Makefile.am to file.
# It uses several steps to obtain the result:
# * Obtain source files between the correct patterns.
# * Remove first line.
# * Remove last line.
# * Remove tabs.
# * Remove new lines.
# * Remove backslashes.
sed -n "/libbibledit_a_SOURCES/,/bin_PROGRAMS/p" Makefile.am | tail -n +2 | sed '$d' | strings | sed 's/\\//g' > sources.txt

# Save the names of the C++ sources to file and load them.
grep .cpp sources.txt > cppfiles.txt
CPPFILES=(`cat cppfiles.txt`)

# Save the name of the C sources to file and load them.
sed '/.cpp/d' sources.txt > cfiles.txt
CFILES=(`cat cfiles.txt`)

for cpp in ${CPPFILES[@]}; do

extension="${cpp##*.}"
basepath="${cpp%.*}"
echo Compiling $cpp

$TOOLDIR/clang++ -arch ${ARCH} -isysroot $SYSROOT -I. $COMPILEFLAGS -std=c++11 -stdlib=libc++ -o $basepath.o $cpp
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


# Copy output to temporal location

pushd webroot
cp libbibledit.a /tmp/libbibledit-$ARCH.a
rm libbibledit.a
popd

}


# Above this point are the functions.
# Below this point starts the main script.


# Take the relevant source code for building Bibledit for iOS.
# Put it in a temporal location.
# The purpose is to put the build files in a temporal location,
# and to have no duplicated code for the bibledit library.
# This does not clutter the bibledit git repository with the built files.
IOSSOURCE=`dirname $0`
cd $IOSSOURCE
BIBLEDITIOS=/tmp/bibledit-ios
echo Synchronizing relevant source code to $BIBLEDITIOS
mkdir -p $BIBLEDITIOS
rsync --archive --delete ../lib $BIBLEDITIOS/
rsync --archive --delete ../ios $BIBLEDITIOS/

# From now on the working directory is the temporal location.
cd $BIBLEDITIOS/ios

# Build the locale databases for inclusion with the iOS package.
# The reason for this is that building them on iOS takes a lot of time during the setup phase.
# So to include pre-built databases speeds up the setup phase of Bibledit on iOS.
# This gives a better user experience.
pushd ../lib
./configure
make --jobs=`sysctl -n hw.ncpu`
./generate . locale
./generate . mappings
./generate . versifications
popd

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
rm -rf sources
rm -rf unittests
popd

pushd webroot
# Configure Bibledit in client mode,
# Run only only one parallel task so the interface is more responsive.
# Enable the single-tab browser.
./configure --with-network-port=8765 --enable-ios
# Update the Makefile.
sed -i.bak '/SWORD_CFLAGS =/d' Makefile
sed -i.bak '/SWORD_LIBS =/d' Makefile
# Update the configuration: No SWORD library.
sed -i.bak '/HAVE_SWORD/d' config.h
# The embedded web view cannot upload files.
sed -i.bak '/CONFIG_ENABLE_FILE_UPLOAD/d' config/config.h
# Done.
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
lipo -create -output /tmp/libbibledit.a /tmp/libbibledit-armv7.a /tmp/libbibledit-armv7s.a /tmp/libbibledit-arm64.a /tmp/libbibledit-i386.a /tmp/libbibledit-x86_64.a
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi
lipo -info /tmp/libbibledit.a
EXIT_CODE=$?
if [ $EXIT_CODE != 0 ]; then
  exit
fi

echo Copying library into place
mv /tmp/libbibledit.a lib

echo Clean libraries from desktop
rm /tmp/libbibledit-armv7.a
rm /tmp/libbibledit-armv7s.a
rm /tmp/libbibledit-arm64.a
rm /tmp/libbibledit-i386.a
rm /tmp/libbibledit-x86_64.a

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
find . -name "*.h" -delete
find . -name "*.cpp" -delete
find . -name "*.c" -delete
find . -name "*.o" -delete
find . -name ".deps" -exec rm -r "{}" \; > /dev/null 2>&1
find . -name ".dirstamp" -delete
rm locale/README
rm sandbox/*
rm -rf unittests
rm -rf sources

popd

# say Compile for iOS is ready

# Build the app.
cd $BIBLEDITIOS/ios
xcodebuild

echo To graphically build the app for iOS open the project in Xcode:
echo open $BIBLEDITIOS/ios/Bibledit.xcodeproj
echo Then build it from within Xcode
