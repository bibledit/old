#!/bin/bash


# Synchronize and build libbibledit on OS X for iOS.


# Configure the Bibledit library.
function configure
{

# Configure it in client mode,
# Run only only one parallel task so the interface is more responsive.
# Enable the single-tab browser.
pushd ../../lib
./configure --enable-client --with-parallel-tasks=1 --enable-single-tab
echo Clean source.
make clean > /dev/null
popd

}


# Sychronizes the libbibledit data files in the source tree to iOS and cleans them up.

configure

rsync -av --delete ../../lib/ ../webroot

pushd ../webroot

make distclean

find . -name "*.h" -delete

find . -name "*.cpp" -delete

find . -name "*.c" -delete

find . -name "*.php" -delete

rm Make*
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
rm -rf .deps

popd


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

CURLINCLUDE=../../dependencies/libcurl/ios/include/curl

pushd $CURLINCLUDE > /dev/null
cp curlbuild$BITS.h curlbuild.h
popd > /dev/null

pushd ../../lib > /dev/null

CPPFILES=(

library/bibledit.cpp
webserver/webserver.cpp
webserver/http.cpp
webserver/request.cpp
bootstrap/bootstrap.cpp
filter/url.cpp
filter/string.cpp
filter/roles.cpp
filter/md5.cpp
filter/usfm.cpp
filter/archive.cpp
filter/text.cpp
filter/passage.cpp
filter/customcss.cpp
filter/bibleworks.cpp
filter/git.cpp
filter/diff.cpp
filter/abbreviations.cpp
filter/shell.cpp
filter/merge.cpp
flate/flate.cpp
assets/view.cpp
assets/page.cpp
assets/header.cpp
index/index.cpp
config/globals.cpp
menu/logic.cpp
menu/main.cpp
menu/user.cpp
locale/translate.cpp
database/config/general.cpp
database/config/bible.cpp
database/config/user.cpp
database/users.cpp
database/logs.cpp
database/sqlite.cpp
database/styles.cpp
database/bibles.cpp
database/search.cpp
database/books.cpp
database/bibleactions.cpp
database/check.cpp
database/commits.cpp
database/confirm.cpp
database/ipc.cpp
database/jobs.cpp
database/kjv.cpp
database/morphhb.cpp
database/sblgnt.cpp
database/offlineresources.cpp
database/sprint.cpp
database/mail.cpp
database/navigation.cpp
database/resources.cpp
database/usfmresources.cpp
database/mappings.cpp
database/noteactions.cpp
database/versifications.cpp
database/modifications.cpp
database/notes.cpp
database/volatile.cpp
database/maintenance.cpp
session/logic.cpp
session/login.cpp
session/logout.cpp
session/password.cpp
session/signup.cpp
parsewebdata/ParseMultipartFormData.cpp
parsewebdata/ParseWebData.cpp
setup/index.cpp
journal/index.cpp
styles/logic.cpp
styles/indext.cpp
styles/indexm.cpp
styles/sheetm.cpp
styles/view.cpp
styles/css.cpp
styles/sheets.cpp
text/text.cpp
esword/text.cpp
onlinebible/text.cpp
html/text.cpp
odf/text.cpp
timer/index.cpp
tasks/logic.cpp
tasks/run.cpp
config/logic.cpp
bible/logic.cpp
bible/manage.cpp
bible/settings.cpp
bible/book.cpp
bible/chapter.cpp
bible/import_usfm.cpp
bible/import_bibleworks.cpp
bible/import_task.cpp
bible/abbreviations.cpp
bible/order.cpp
bible/css.cpp
bible/editing.cpp
notes/logic.cpp
trash/handler.cpp
sync/logic.cpp
help/index.cpp
confirm/worker.cpp
email/index.cpp
email/send.cpp
email/receive.cpp
user/notifications.cpp
user/account.cpp
manage/indexing.cpp
manage/users.cpp
administration/language.cpp
administration/timezone.cpp
collaboration/index.cpp
collaboration/flash.cpp
collaboration/open.cpp
collaboration/password.cpp
collaboration/secure.cpp
collaboration/link.cpp
collaboration/direction.cpp
search/rebibles.cpp
search/renotes.cpp
access/user.cpp
access/bible.cpp
dialog/entry.cpp
dialog/list.cpp
dialog/yes.cpp
dialog/color.cpp
dialog/books.cpp
dialog/upload.cpp
fonts/logic.cpp
fonts/index.cpp
versification/index.cpp
versification/system.cpp
book/create.cpp
compare/index.cpp
compare/compare.cpp
jobs/index.cpp
editverse/index.cpp
editverse/id.cpp
editverse/load.cpp
editverse/save.cpp
navigation/passage.cpp
navigation/update.cpp
navigation/poll.cpp
ipc/focus.cpp
checksum/logic.cpp
editusfm/focus.cpp
editusfm/id.cpp
editusfm/index.cpp
editusfm/load.cpp
editusfm/offset.cpp
editusfm/save.cpp
editor/styles.cpp
editor/export.cpp
editor/import.cpp
edit/edit.cpp
edit/focus.cpp
edit/id.cpp
edit/index.cpp
edit/load.cpp
edit/offset.cpp
edit/save.cpp
edit/styles.cpp
search/search.cpp
search/index.cpp
search/select.cpp
search/replace.cpp
search/getids.cpp
search/replacepre.cpp
search/replacego.cpp
search/search2.cpp
search/replace2.cpp
search/replacepre2.cpp
search/getids2.cpp
search/replacego2.cpp
search/similar.cpp
search/strongs.cpp
search/strong.cpp
search/originals.cpp
tmp/tmp.cpp
workbench/index.cpp
workbench/logic.cpp
workbench/organize.cpp
workbench/settings.cpp
workbench/organize.cpp
sendreceive/logic.cpp
sendreceive/index.cpp
sendreceive/sendreceive.cpp
sendreceive/settings.cpp
sendreceive/bibles.cpp
demo/logic.cpp
client/index.cpp
client/logic.cpp
sync/setup.cpp
sync/settings.cpp
sync/bibles.cpp

)

CFILES=(

webserver/io.c

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

popd > /dev/null

pushd $CURLINCLUDE > /dev/null
rm curlbuild.h
popd > /dev/null


# Linking
echo Linking

pushd ../../lib > /dev/null

rm -f libbibledit.a

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

popd > /dev/null


# Copy output to desktop

pushd ../../lib > /dev/null
cp libbibledit.a ~/Desktop/libbibledit-$ARCH.a
popd > /dev/null

}


configure
compile armv7 iPhoneOS 32

configure
compile armv7s iPhoneOS 32

configure
compile arm64 iPhoneOS 64

configure
compile i386 iPhoneSimulator 32

configure
compile x86_64 iPhoneSimulator 64

cp ../../lib/library/bibledit.h ../include/

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
mv ~/Desktop/libbibledit.a ../lib

echo Clean libraries from desktop
rm ~/Desktop/libbibledit-armv7.a
rm ~/Desktop/libbibledit-armv7s.a
rm ~/Desktop/libbibledit-arm64.a
rm ~/Desktop/libbibledit-i386.a
rm ~/Desktop/libbibledit-x86_64.a

say Compile for iOS is ready

echo Restore library to default state
unset IPHONEOS_DEPLOYMENT_TARGET
unset SYSROOT
unset TOOLDIR
unset COMPILEFLAGS
unset CURLINCLUDE
pushd ../../lib
./configure
make clean > /dev/null
make --jobs=4
popd
