#!/bin/bash


# This script runs in a Terminal on OS X.
# It refreshes and updates the bibledit sources.
# It builds a tar ball for use on Linux.


# Be sure to start off in the correct directory.
cd `dirname $0`


# Synchronize the Bibledit core library to the Linux folder.
cd ..
rsync --archive --exclude xcode ../lib/ .
./configure
make distclean


# Update the source for the configuration.
sed -i.bak 's/ENABLELINUX=no/ENABLELINUX=yes/g' configure.ac
sed -i.bak 's/.*Tag1.*/PKG_CHECK_MODULES(GTK, gtk+-3.0,,AC_MSG_ERROR(Gtk3 development version is needed.))/g' configure.ac
sed -i.bak 's/.*Tag2.*/AC_SUBST(GTK_LIBS)/g' configure.ac
sed -i.bak 's/.*Tag3.*/AC_SUBST(GTK_CFLAGS)/g' configure.ac
sed -i.bak 's/.*Tag4.*/AC_DEFINE([HAVE_LINUX], [1], [Enable installation on Linux])/g' configure.ac


# Update the source for the Makefile.
sed -i.bak 's/server unittest/bibledit/g' Makefile.am
sed -i.bak 's/bin_PROGRAMS/noinst_PROGRAMS/g' Makefile.am
sed -i.bak 's/server_/bibledit_/g' Makefile.am
sed -i.bak '/unittest/d' Makefile.am
sed -i.bak '/AM_CXXFLAGS =/ s/$/ $(GTK_CFLAGS)/' Makefile.am
sed -i.bak '/LIBS =/ s/$/ $(GTK_LIBS)/' Makefile.am
sed -i.bak 's/bible bibledit/bible/g' Makefile.am
sed -i.bak '/EXTRA_DIST/ s/$/ *.desktop *.xpm *.sh *.1/' Makefile.am
echo '' >> Makefile.am
echo 'man_MANS = *.1' >> Makefile.am
echo '' >> Makefile.am
echo 'applicationdir = $(datadir)/applications' >> Makefile.am
echo 'application_DATA = bibledit.desktop' >> Makefile.am
echo '' >> Makefile.am
echo 'appicondir = $(datadir)/pixmaps' >> Makefile.am
echo 'appicon_DATA = bibledit.xpm' >> Makefile.am
echo '' >> Makefile.am
echo 'bin_SCRIPTS = bibledit.sh' >> Makefile.am
# Remove consecutive blank lines.
sed -i.bak '/./,/^$/!d' Makefile.am


# Copy the GUI sources into place.
cp linux/bibledit.h executable
cp linux/bibledit.cpp executable
cp linux/bibledit.1 .
cp linux/bibledit.xpm .
cp linux/bibledit.desktop .
cp linux/bibledit.sh .


# Remove unnecessary programs.
rm valgrind
rm bibledit
rm dev
rm *.bak


./reconfigure

make distclean

./configure --enable-client --enable-paratext

make dist

