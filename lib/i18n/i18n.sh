#!/bin/bash


echo Setting working directory.
cd "$(dirname "$0")"


echo Gathering all the html files for internationalization.
find .. -iname "*.html" > i18n.html
if [ $? -ne 0 ]
then
echo Cannot find source files
fi


g++ -std=c++11 i18n.cpp -o i18n
./i18n
rm i18n


echo Create a temporal file containing all the files for internationalization.
find ~/dev/bibledit/lib -iname "*.cpp" -o -iname "books.h" > gettextfiles.txt
if [ $? -ne 0 ]
then
echo Cannot find source files
fi


echo Remove files not to be processed.
sed -i.bak '/utf8/d' gettextfiles.txt
sed -i.bak '/unittests/d' gettextfiles.txt


echo Remove bibledit.pot in case there are some strings in it which are no longer in use.
rm -f /tmp/bibledit.pot


echo Extracting translatable strings and storing them in bibledit.pot
xgettext --files-from=gettextfiles.txt --default-domain=bibledit --force-po --copyright-holder="Teus Benschop" -o /tmp/bibledit.pot --from-code=UTF-8 --no-location --keyword=translate --language=C
if [ $? -ne 0 ]
then
echo Failure running xgettext
fi


echo Copying bibledit.pot into place
cp /tmp/bibledit.pot ../locale


echo Clean temporal files.
rm gettextfiles*
rm /tmp/bibledit.pot
rm translatables.cpp
rm i18n.html


echo Pull translations from launchpad.net
cd
cd dev/bibledit.pot
bzr pull lp:~teusbenschop/bibledit/translations
if [ $? -ne 0 ]
then
echo Could not pull translations from launchpad.net
fi


echo Push new translatable messages to Launchpad.
cd
cd dev/bibledit.pot
cp ../bibledit/lib/locale/bibledit.pot gtk/po/
bzr add gtk/po/bibledit.pot
bzr commit --message "updated bibledit.pot"


echo Synchronize translations to Bibledit
cd
cd dev/bibledit.pot/gtk/po
rsync -av . ~/dev/bibledit/lib/locale
if [ $? -ne 0 ]
then
echo Could not synchronize translations to Bibledit
fi


echo Clean dates out.
sed -i.bak '/POT-Creation-Date/d' ~/dev/bibledit/lib/locale/*.po ~/dev/bibledit/lib/locale/bibledit.pot
sed -i.bak '/X-Launchpad-Export-Date/d' ~/dev/bibledit/lib/locale/*.po ~/dev/bibledit/lib/locale/bibledit.pot
rm ~/dev/bibledit/lib/locale/*.bak


