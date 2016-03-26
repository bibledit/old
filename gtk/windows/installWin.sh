#!/bin/bash
# Copy all necessary files from git repo to Windows to make a "nice" installation.
# Run this from the bibledit/gtk build directory
echo "I assume you are running this from your-home/bibledit/gtk."
echo "I also assume this shell is running in elevated/administrator mode!"
echo "If not, hit Ctrl-C now and correct those problems!"
sleep 3

VERSION="4.9.3"

#------------------------------------------------------------------------------------------
# Figure out if we are in a 32-bit or 64-bit environment and target Windows accordingly
#------------------------------------------------------------------------------------------
if [ -n "$MSYSTEM" ]
then
  case "$MSYSTEM" in
    MINGW32)
      # Program Files (x86) for 32-bit programs
	  PROGRAMFILES="Program Files (x86)"
	  # Source directory for binaries and such
	  MINGWDIR="mingw32"
    ;;
    MINGW64)
	  # Program Files for 64-bit programs
      PROGRAMFILES="Program Files"
	  # Source directory for binaries and such
	  MINGWDIR="mingw64"
    ;;
    MSYS)
      echo "I am not set up to install from an msys shell. Please use"
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

# Because of the space in Program Files, I have to quote this carefully
# when I use it.
BIN="/c/$PROGRAMFILES/Bibledit-$VERSION/editor/bin"
SHARE="/c/$PROGRAMFILES/Bibledit-$VERSION/editor/share"
LIB="/c/$PROGRAMFILES/Bibledit-$VERSION/editor/lib"
# Take note: this is the 64-bit version of stuff
DLLS="/$MINGWDIR/bin"
THEMES="/$MINGWDIR/share/themes"
ENGINES="/$MINGWDIR/lib/gtk-2.0"
MINGWBIN="/$MINGWDIR/bin"
USRBIN="/usr/bin"

echo "Copying Bibledit executables to $BIN..."
mkdir -v -p "$BIN"
cp ./src/.libs/bibledit-gtk.exe "$BIN"
cp ./src/.libs/bibledit-rdwrt.exe "$BIN"
cp ./git/bibledit-git.exe "$BIN"
cp ./shutdown/bibledit-shutdown.exe "$BIN"

#What about bwoutpost.exe???? We need to be building that.
#which: no egrep.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no fgrep.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no funzip.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no pgawk.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no pgawk-3.1.7.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no rcsclean.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no rcsdiff.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#hich: no rcsmerge.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no unzip.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no unzipsfx.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no zip.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no zipcloak.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no zipinfo.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no zipnote.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
#which: no zipsplit.exe in (/mingw64/bin:/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/Intel/iCLS Client:/c/Program Files/Intel/iCLS Client:/c/Windows/system32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0:/c/Program Files/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/DAL:/c/Program Files/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/Intel/Intel(R) Management Engine Components/IPT:/c/Program Files (x86)/ATI Technologies/ATI.ACE/Core-Static:/c/Program Files/Lame3.99.5-64:/c/Program Files (x86)/GNU/GnuPG/pub:/c/Strawberry/c/bin:/c/Strawberry/perl/site/bin:/c/Strawberry/perl/bin:/c/Program Files (x86)/Windows Kits/8.1/Windows Performance Toolkit:/c/Program Files (x86)/AMD/ATI.ACE/Core-Static:/c/Program Files/Intel/WiFi/bin:/c/Program Files/Common Files/Intel/WirelessCommon:/c/Program Files (x86)/Skype/Phone:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
# Need to find a windows php5-cli. Tough to build in mingw64/bin
# Need to find a texlive-xetex package for Windows. Too hard.
# Need to develop a way to get bwoutpost.executable in the installation (maybe put it on fbcaa.org), until we can replace it.

echo "Copying libgcc from $DLLS to $BIN"
if [ -e "$DLLS/libgcc_s_seh-1.dll" ]
then
  echo "  64-bit: Copying libgcc_s_seh-1"
  cp $DLLS/libgcc_s_seh-1.dll "$BIN"
fi
if [ -e "$DLLS/libgcc_s_dw2-1.dll" ]
then
  echo "  32-bit: Copying libgcc_s_dw2-1"
  cp $DLLS/libgcc_s_dw2-1.dll "$BIN"
fi

echo "Copying DLLs from $DLLS to $BIN..."
cp $DLLS/libcairo-2.dll "$BIN"
cp $DLLS/libenchant.dll "$BIN"
cp $DLLS/libgdk-win32-2.0-0.dll "$BIN"
cp $DLLS/libglib-2.0-0.dll "$BIN"
cp $DLLS/libgobject-2.0-0.dll "$BIN"
cp $DLLS/libgtk-win32-2.0-0.dll "$BIN"
cp $DLLS/libgtksourceview-2.0-0.dll "$BIN"
cp $DLLS/libintl-8.dll "$BIN"
cp $DLLS/libpango-1.0-0.dll "$BIN" 
cp $DLLS/libpangocairo-1.0-0.dll "$BIN"
cp $DLLS/libsoup-2.4-1.dll "$BIN"
cp $DLLS/libsqlite3-0.dll "$BIN"
cp $DLLS/libwebkitgtk-1.0-0.dll "$BIN"
cp $DLLS/libxml2-2.dll "$BIN"
cp $DLLS/libatk-1.0-0.dll "$BIN"
cp $DLLS/libffi-6.dll "$BIN"
cp $DLLS/libfontconfig-1.dll "$BIN"
cp $DLLS/libfreetype-6.dll "$BIN"
cp $DLLS/libgdk_pixbuf-2.0-0.dll "$BIN"
cp $DLLS/libgio-2.0-0.dll "$BIN"
cp $DLLS/libgmodule-2.0-0.dll "$BIN"
cp $DLLS/libgstapp-1.0-0.dll "$BIN"
cp $DLLS/libgstaudio-1.0-0.dll "$BIN"
cp $DLLS/libgstbase-1.0-0.dll "$BIN"
cp $DLLS/libgstfft-1.0-0.dll "$BIN"
cp $DLLS/libgstpbutils-1.0-0.dll "$BIN"
cp $DLLS/libgstreamer-1.0-0.dll "$BIN"
cp $DLLS/libgstvideo-1.0-0.dll "$BIN"
cp $DLLS/libharfbuzz-0.dll "$BIN"
cp $DLLS/libharfbuzz-ICU-0.dll "$BIN"
cp $DLLS/libiconv-2.dll "$BIN"
cp $DLLS/libicuin56.dll "$BIN"
cp $DLLS/libicuuc56.dll "$BIN"
cp $DLLS/libjavascriptcoregtk-1.0-0.dll "$BIN"
cp $DLLS/libjpeg-8.dll "$BIN"
cp $DLLS/libbz2-1.dll "$BIN"
cp $DLLS/libexpat-1.dll "$BIN"
cp $DLLS/libgeoclue-0.dll "$BIN"
cp $DLLS/libgsttag-1.0-0.dll "$BIN"
cp $DLLS/libicudt56.dll "$BIN"
cp $DLLS/liblzma-5.dll "$BIN"
cp $DLLS/liborc-0.4-0.dll "$BIN"
cp $DLLS/libpangoft2-1.0-0.dll "$BIN"
cp $DLLS/libpangowin32-1.0-0.dll "$BIN"
cp $DLLS/libpixman-1-0.dll "$BIN"
cp $DLLS/libpng16-16.dll "$BIN"
cp $DLLS/libwebp-6.dll "$BIN"
cp $DLLS/libxslt-1.dll "$BIN"
cp $DLLS/libdbus-glib-1-2.dll "$BIN"
cp $DLLS/libdbus-1-3.dll "$BIN"
cp $DLLS/libstdc++-6.dll "$BIN"
cp $DLLS/zlib1.dll "$BIN"
cp $DLLS/libwinpthread-1.dll "$BIN"
# Below added 3/24/2016. Very important to have one or more of these
cp $DLLS/libexslt-0.dll "$BIN"
cp $DLLS/libgailutil-18.dll "$BIN"
cp $DLLS/libgailutil-3-0.dll "$BIN"
cp $DLLS/libgmp-10.dll "$BIN"
cp $DLLS/libgmpxx-4.dll "$BIN"
cp $DLLS/libgnutls-30.dll "$BIN"
cp $DLLS/libgnutlsxx-28.dll "$BIN"
cp $DLLS/libnettle-6-1.dll "$BIN"
cp $DLLS/libtiff-5.dll "$BIN"
cp $DLLS/libtiffxx-5.dll "$BIN"
cp $DLLS/libp11-kit-0.dll "$BIN"
cp $DLLS/libtasn1-6.dll "$BIN"
# Above added 3/24/2016.
# Below added 3/25/2016
cp $DLLS/libhogweed-4-1.dll "$BIN"
cp $DLLS/libjasper-1.dll "$BIN"
cp $DLLS/libgthread-2.0-0.dll "$BIN"
cp $DLLS/libhunspell-1.3-0.dll "$BIN"
cp $DLLS/libpng-config "$BIN"
cp $DLLS/libpng16-16.dll "$BIN"
cp $DLLS/libpng16-config "$BIN"
cp $DLLS/libicuuc56.dll "$BIN"
cp $DLLS/libicutu56.dll "$BIN"
cp $DLLS/libicutest56.dll "$BIN"
cp $DLLS/libiculx56.dll "$BIN"
cp $DLLS/libicule56.dll "$BIN"
cp $DLLS/libicuio56.dll "$BIN"
cp $DLLS/icu-config "$BIN"
# Above added 3/25/2016

echo "Copying various executables to $BIN"
cp $USRBIN/bison.exe "$BIN"
cp $USRBIN/cat.exe "$BIN"
cp $USRBIN/cmp.exe "$BIN"
cp $USRBIN/cp.exe "$BIN"
cp $USRBIN/curl.exe "$BIN"
cp $USRBIN/date.exe "$BIN"
cp $USRBIN/diff.exe "$BIN"
cp $USRBIN/diff3.exe "$BIN"
cp $USRBIN/echo.exe "$BIN"
cp $USRBIN/false.exe "$BIN"
cp $USRBIN/find.exe "$BIN"
cp $USRBIN/flex.exe "$BIN"
cp $USRBIN/gawk.exe "$BIN"
cp $USRBIN/gperf.exe "$BIN"
cp $USRBIN/grep.exe "$BIN"
cp $USRBIN/gzip.exe "$BIN"
cp $USRBIN/iconv.exe "$BIN"
cp $USRBIN/info.exe "$BIN"
cp $USRBIN/join.exe "$BIN"
cp $USRBIN/less.exe "$BIN"
cp $USRBIN/lessecho.exe "$BIN"
cp $USRBIN/lesskey.exe "$BIN"
cp $USRBIN/ln.exe "$BIN"
cp $USRBIN/ls.exe "$BIN"
cp $USRBIN/md5sum.exe "$BIN"
cp $USRBIN/merge.exe "$BIN"
cp $USRBIN/mkdir.exe "$BIN"
cp $USRBIN/mv.exe "$BIN"
cp $USRBIN/paste.exe "$BIN"
cp $USRBIN/patch.exe "$BIN"
cp $USRBIN/printf.exe "$BIN"
cp $USRBIN/ps.exe "$BIN"
cp $USRBIN/pwd.exe "$BIN"
cp $USRBIN/rcs.exe "$BIN"
cp $USRBIN/rm.exe "$BIN"
cp $USRBIN/rmdir.exe "$BIN"
cp $USRBIN/sdiff.exe "$BIN"
cp $USRBIN/sed.exe "$BIN"
cp $USRBIN/sleep.exe "$BIN"
cp $USRBIN/sort.exe "$BIN"
cp $USRBIN/split.exe "$BIN"
cp $USRBIN/tail.exe "$BIN"
cp $USRBIN/tar.exe "$BIN"
cp $USRBIN/tee.exe "$BIN"
cp $USRBIN/touch.exe "$BIN"
cp $USRBIN/tr.exe "$BIN"
cp $USRBIN/true.exe "$BIN"
cp $USRBIN/uname.exe "$BIN"
cp $USRBIN/wc.exe "$BIN"
cp $USRBIN/wget.exe "$BIN"
cp $USRBIN/xargs.exe "$BIN"
cp $USRBIN/xml2ag.exe "$BIN"
cp $USRBIN/msys-intl-8.dll "$BIN"
cp $USRBIN/msys-iconv-2.dll "$BIN"
cp $USRBIN/msys-2.0.dll "$BIN"
cp $MINGWBIN/nm.exe "$BIN"
cp $MINGWBIN/bunzip2.exe "$BIN"
cp $MINGWBIN/bzip2.exe "$BIN"
cp $MINGWBIN/size.exe "$BIN"
cp $MINGWBIN/sqlite3.exe "$BIN"
cp $MINGWBIN/strings.exe "$BIN"
cp $MINGWBIN/strip.exe "$BIN"
cp $MINGWBIN/unxz.exe "$BIN"
cp $MINGWBIN/xz.exe "$BIN"
cp $MINGWBIN/xzcat.exe "$BIN"
cp $MINGWBIN/xzdec.exe "$BIN"
# Below added 3/25/2016
cp $USRBIN/msys-icudata56.dll "$BIN"
cp $USRBIN/msys-icui18n56.dll "$BIN"
# Above added 3/25/2016

# The DLL ieshims.dll is a special case. depends.exe says we need it, but we don't.

echo "Copying themes and engines..."
mkdir -v -p "$SHARE/themes"
cp -R $THEMES/* "$SHARE/themes/"
mkdir -v -p "$LIB/gtk-2.0"
cp -R $ENGINES/* "$LIB/gtk-2.0/"

echo "Copying templates, etc. to $SHARE/bibledit..."
mkdir -v -p "$SHARE/bibledit"
cp -R templates/* "$SHARE/bibledit"
cp -R pix/* "$SHARE/bibledit"
cp -R usfm/usfm.sty "$SHARE/bibledit"
cp -R doc/site "$SHARE/bibledit"
cp -R doc/index.html "$SHARE/bibledit"
cp -R doc/menus.pl "$SHARE/bibledit"
cp -R doc/retrieval.pl "$SHARE/bibledit"
cp -R doc/style.css "$SHARE/bibledit"
cp -R doc/site.xml "$SHARE/bibledit"

echo "Fetching bwoutpost.exe and installing to $BIN"
wget http://fbcaa.org/bibledit/bwoutpost.exe
mv -f bwoutpost.exe "$BIN"

# I am missing the larger icon bibledit.ico - I think I have it now, but not in the binary?

echo "Done..."