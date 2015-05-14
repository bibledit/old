#!/bin/bash


# This script runs in a Terminal on OS X.
# It refreshes and updates the bibledit sources.
# It synchronizes all relevant data to a Linux host.

pushd ..

rsync -av --exclude "*.o" --exclude ".deps" --exclude xcode --exclude ".dirstamp" --exclude "*~" --exclude "autom4te.cache" --exclude "*.a" ../lib/ .

rm valgrind
rm -f unittest
rm -f server
rm bibledit
rm dev

sed -i.bak 's/CLIENT=no/CLIENT=yes/g' configure.ac
sed -i.bak 's/.*Tag1.*/PKG_CHECK_MODULES(GTK, gtk+-3.0,,AC_MSG_ERROR(Gtk3 development version is needed.))/g' configure.ac
sed -i.bak 's/.*Tag2.*/AC_SUBST(GTK_LIBS)/g' configure.ac
sed -i.bak 's/.*Tag3.*/AC_SUBST(GTK_CFLAGS)/g' configure.ac

sed -i.bak 's/server unittest/bibledit/g' Makefile.am
sed -i.bak 's/server_/bibledit_/g' Makefile.am
sed -i.bak '/unittest/d' Makefile.am
sed -i.bak '/AM_CXXFLAGS =/ s/$/ $(GTK_CFLAGS)/' Makefile.am
sed -i.bak '/LIBS =/ s/$/ $(GTK_LIBS)/' Makefile.am
sed -i.bak 's/bible bibledit/bible/g' Makefile.am
sed -i.bak '/EXTRA_DIST/ s/$/ *.desktop *.xpm/' Makefile.am
echo '' >> Makefile.am
echo 'man_MANS = *.1' >> Makefile.am
# Remove consecutive blank lines.
sed -i.bak '/./,/^$/!d' Makefile.am

cp linux/bibledit* executable

./reconfigure

rsync -av . 192.168.2.12:./bibledit

popd
