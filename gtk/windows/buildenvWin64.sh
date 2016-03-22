#!/bin/bash

#------------------------------------------------------------------------------------------
# Figure out if we are in a 32-bit or 64-bit environment and setup Windows build 
# environment accordingly. I am assuming this is built on a 64-bit machine, i.e. that
# you have installed msys2-x86_64...exe. In other words, the packages are always
# $MINGWDIR/mingw-w64...
#------------------------------------------------------------------------------------------
if [ -n "$MSYSTEM" ]
then
  case "$MSYSTEM" in
    MINGW32)
      # Program Files (x86) for 32-bit programs
	  PLATFORM="i686"
	  BUILD_DIR="32-bit"
	  # Source directory for binaries and such
	  MINGWDIR="mingw32"
    ;;
    MINGW64)
	  # Program Files for 64-bit programs
      PLATFORM="x86_64"
	  BUILD_DIR="64-bit"
	  # Source directory for binaries and such
	  MINGWDIR="mingw64"
    ;;
    MSYS)
      echo "I am not smart enough to install from an msys shell. Please use"
	  echo "a MINGW32 or MINGW64 shell."
	  exit 1
    ;;
    *)
      echo "Unknown value $MSYSTEM in MSYSTEM environment variable, so I don't know what to do."
      exit 2
    ;;
  esac
else
  echo "Can't find MSYSTEM environment variable, so I don't know what to do."
  exit 3
fi

pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-gedit
# All msys packages
pacman -S --noconfirm base-devel
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-gtk2
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-gtkmm
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-gtkmm3
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-python2-pygtk
pacman -S --noconfirm msys/gtk-doc
pacman -S --noconfirm msys/git
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-sqlite3 msys/libsqlite-devel msys/sqlite msys/sqlite-doc
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-gtksourceview2 $MINGWDIR/mingw-w64-$PLATFORM-gtksourceviewmm3
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-webkitgtk2 $MINGWDIR/mingw-w64-$PLATFORM-webkitgtk3
pacman -S --noconfirm $MINGWDIR/mingw-w64-$PLATFORM-emacs
# The next also installs liblzma-devel-5.2.1-1  libreadline-devel-6.3.008-7
# ncurses-devel-6.0.20160220-1  zlib-devel-1.2.8-3
pacman -S --noconfirm msys/libxml2-devel
mkdir -v -p ~/$BUILD_DIR
cd ~/$BUILD_DIR
git clone https://github.com/teusbenschop/bibledit.git

echo "Git cloned into the directory ~/$BUILD_DIR/bibledit"

checkForZLIB1()
{
if [ -e $1 ]
then
    echo "*****************************************************************"
    echo "I found zlib1.dll at $1. This might cause"
	echo "a lot of problems when you run configure and come to the check"
	echo "for libxml2 or when you run gcc and it doesn't produce any output."
	echo "That copy of zlib1.dll will almost certainly need to be removed."
	echo "I know...you say what does zlib1.dll have to do with gcc writes"
	echo "or libxml2? It is a long story. MAP 3/21/2016."
	echo "*****************************************************************"
fi
}

checkForZLIB1 "/c/Windows/system32/zlib1.dll"
checkForZLIB1 "/c/Windows/SysWOW64/zlib1.dll"
