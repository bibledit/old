#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  sudo "$0"
  exit
fi

echo "This script works on ubuntu Linux 14.04 LTS and Arch Linux"

# I assume you have already done this:
# sudo apt-get install git
# cd ~
# git clone https://github.com/teusbenschop/bibledit.git

if apt-get --version 2> /dev/null; then
  apt-get install gitk
  apt-get install build-essential
  apt-get install libgtk2.0-dev
  apt-get install rcs
  apt-get install sqlite3
  apt-get install libglibmm-2.4-dev libglibmm-2.4
  apt-get install libsqlite3-dev
  apt-get install libxml2-dev
  apt-get install libenchant-dev
  apt-get install libgtkhtml3.14-dev
  apt-get install libgtksourceview2.0-dev
  apt-get install libwebkit-dev
  apt-get install texlive-xetex
  apt-get install php5-cli
  apt-get install curl
  apt-get install intltool
  apt-get install libtool
  apt-get install autoconf-archive
elif pacman --version 2> /dev/null; then
  # TODO: investigate if gtkhtml3 is necessary (it is in the AUR)
  pacman -S --needed --noconfirm base-devel gtk2 rcs sqlite \
    glibmm sqlite libxml2 enchant libgtksourceviewmm2 \
    texlive-core php curl intltool libtool autoconf-archive \
    webkitgtk2
fi
