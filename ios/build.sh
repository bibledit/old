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

CPPFILES=(

library/bibledit.cpp
webserver/webserver.cpp
webserver/http.cpp
webserver/request.cpp
bootstrap/bootstrap.cpp
filter/UriCodec.cpp
filter/abbreviations.cpp
filter/archive.cpp
filter/css.cpp
filter/date.cpp
filter/diff.cpp
filter/git.cpp
filter/md5.cpp
filter/merge.cpp
filter/passage.cpp
filter/roles.cpp
filter/shell.cpp
filter/string.cpp
filter/text.cpp
filter/url.cpp
filter/usfm.cpp
flate/flate.cpp
assets/view.cpp
assets/page.cpp
assets/header.cpp
index/index.cpp
index/listing.cpp
config/globals.cpp
menu/logic.cpp
menu/index.cpp
locale/translate.cpp
locale/logic.cpp
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
database/usfmresources.cpp
database/mappings.cpp
database/noteactions.cpp
database/versifications.cpp
database/modifications.cpp
database/notes.cpp
database/volatile.cpp
database/maintenance.cpp
database/localization.cpp
database/state.cpp
database/noteassignment.cpp
database/imageresources.cpp
database/etcbc4.cpp
database/hebrewlexicon.cpp
database/morphgnt.cpp
database/strong.cpp
database/cache.cpp
session/logic.cpp
session/login.cpp
session/logout.cpp
session/password.cpp
session/signup.cpp
parsewebdata/ParseMultipartFormData.cpp
parsewebdata/ParseWebData.cpp
setup/index.cpp
setup/logic.cpp
journal/index.cpp
journal/logic.cpp
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
html/header.cpp
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
bible/import.cpp
bible/import_run.cpp
bible/abbreviations.cpp
bible/order.cpp
bible/css.cpp
bible/editing.cpp
trash/handler.cpp
help/index.cpp
confirm/worker.cpp
email/index.cpp
email/send.cpp
email/receive.cpp
user/notifications.cpp
user/account.cpp
manage/users.cpp
manage/exports.cpp
manage/hyphenate.cpp
manage/hyphenation.cpp
manage/write.cpp
manage/index.cpp
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
versification/index.cpp
versification/logic.cpp
versification/system.cpp
book/create.cpp
compare/index.cpp
compare/compare.cpp
jobs/index.cpp
editverse/index.cpp
editverse/load.cpp
editverse/save.cpp
navigation/passage.cpp
navigation/update.cpp
navigation/poll.cpp
ipc/focus.cpp
ipc/notes.cpp
checksum/logic.cpp
editusfm/focus.cpp
editusfm/index.cpp
editusfm/load.cpp
editusfm/offset.cpp
editusfm/save.cpp
editor/styles.cpp
editor/usfm2html.cpp
editor/html2usfm.cpp
edit/edit.cpp
edit/focus.cpp
edit/id.cpp
edit/index.cpp
edit/load.cpp
edit/offset.cpp
edit/save.cpp
edit/styles.cpp
edit/logic.cpp
search/index.cpp
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
search/all.cpp
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
sendreceive/files.cpp
sendreceive/notes.cpp
sendreceive/changes.cpp
sendreceive/resources.cpp
demo/logic.cpp
client/index.cpp
client/logic.cpp
sync/bibles.cpp
sync/externalresources.cpp
sync/logic.cpp
sync/notes.cpp
sync/settings.cpp
sync/setup.cpp
sync/usfmresources.cpp
sync/changes.cpp
sync/files.cpp
sync/resources.cpp
resource/admin.cpp
resource/bible2resource.cpp
resource/download.cpp
resource/logic.cpp
resource/external.cpp
resource/manage.cpp
resource/convert2bible.cpp
resource/get.cpp
resource/organize.cpp
resource/convert2resource.cpp
resource/index.cpp
resource/print.cpp
resource/imagefetch.cpp
resource/images.cpp
resource/image.cpp
resource/img.cpp
resource/sword.cpp
resource/select.cpp
checks/headers.cpp
checks/index.cpp
checks/logic.cpp
checks/run.cpp
checks/sentences.cpp
checks/settings.cpp
checks/settingspatterns.cpp
checks/settingssentences.cpp
checks/space.cpp
checks/suppress.cpp
checks/usfm.cpp
checks/verses.cpp
checks/versification.cpp
notes/actions.cpp
notes/assign-1.cpp
notes/assign-n.cpp
notes/bible-1.cpp
notes/bible-n.cpp
notes/bulk.cpp
notes/clean.cpp
notes/cleancli.cpp
notes/click.cpp
notes/comment.cpp
notes/create.cpp
notes/edit.cpp
notes/index.cpp
notes/logic.cpp
notes/note.cpp
notes/notes.cpp
notes/poll.cpp
notes/select.cpp
notes/severity-1.cpp
notes/severity-n.cpp
notes/status-1.cpp
notes/status-n.cpp
notes/summary.cpp
notes/unassign-n.cpp
notes/verses.cpp
xrefs/clear.cpp
xrefs/extract.cpp
xrefs/help.cpp
xrefs/index.cpp
xrefs/insert.cpp
xrefs/interpret.cpp
xrefs/move.cpp
xrefs/next.cpp
xrefs/source.cpp
xrefs/target.cpp
xrefs/translate.cpp
debug/index.cpp
export/esword.cpp
export/html.cpp
export/index.cpp
export/info.cpp
export/logic.cpp
export/odt.cpp
export/onlinebible.cpp
export/textusfm.cpp
export/usfm.cpp
export/web.cpp
export/bibledropbox.cpp
mapping/index.cpp
mapping/map.cpp
editone/index.cpp
editone/load.cpp
editone/save.cpp
editone/verse.cpp
sprint/burndown.cpp
sprint/index.cpp
changes/change.cpp
changes/changes.cpp
changes/logic.cpp
changes/manage.cpp
changes/modifications.cpp
webbible/search.cpp
consistency/index.cpp
consistency/input.cpp
consistency/logic.cpp
consistency/poll.cpp
statistics/statistics.cpp
paratext/index.cpp
paratext/logic.cpp
personalize/index.cpp
lexicon/logic.cpp
lexicon/definition.cpp
sword/logic.cpp
public/chapter.cpp
public/login.cpp
public/comment.cpp
public/new.cpp
public/create.cpp
public/note.cpp
public/index.cpp
public/notes.cpp
public/logic.cpp
pugixml/pugixml.cpp
pugixml/utils.cpp

)

CFILES=(

webserver/io.c
json/json.c
library/locks.c
utf8proc/utf8proc.c

)

for cpp in ${CPPFILES[@]}; do

extension="${cpp##*.}"
basepath="${cpp%.*}"
echo Compiling $cpp

$TOOLDIR/clang++ -arch ${ARCH} -isysroot $SYSROOT -I. -I/usr/include/libxml2 $COMPILEFLAGS -std=c++11 -stdlib=libc++ -o $basepath.o $cpp
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
./configure --enable-client --with-parallel-tasks=3 --enable-bare-browser --enable-tinyjournal --with-network-port=8765
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

say Compile for iOS is ready
