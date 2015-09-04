#!/bin/bash

echo Installing Bibledit on Linux.

echo Please provide the password for the administrative user and press Enter:
read PASSWORD
echo $PASSWORD

clear
echo Updating the software sources...
echo apt-get update
sleep 1
echo $PASSWORD | sudo -S apt-get update
sleep 4

clear
echo Installing the software Bibledit relies on...
echo apt-get install build-essential git zip pkgconf libxml2-dev libsqlite3-dev libcurl4-openssl-dev libssl-dev libatspi2.0-dev libgtk-3-dev libicu-dev
sleep 1
echo $PASSWORD | sudo -S apt-get --yes --force-yes install build-essential
echo $PASSWORD | sudo -S apt-get --yes --force-yes install git
echo $PASSWORD | sudo -S apt-get --yes --force-yes install zip
echo $PASSWORD | sudo -S apt-get --yes --force-yes install pkgconf
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libxml2-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libsqlite3-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libcurl4-openssl-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libssl-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libatspi2.0-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libgtk-3-dev
echo $PASSWORD | sudo -S apt-get --yes --force-yes install libicu-dev
sleep 4

clear
echo Downloading Bibledit...
echo cd
echo wget --continue http://bibledit.org/linux/bibledit-1.0.228.tar.gz
cd
wget --continue http://bibledit.org/linux/bibledit-1.0.228.tar.gz
if [ $? -ne 0 ]
then
  echo Failed to download Bibledit
  exit
fi
sleep 4

clear
echo Unpacking Bibledit in folder bibledit...
echo mkdir -p bibledit
echo tar xf bibledit-1.0.228.tar.gz -C bibledit --strip-components=1
mkdir -p bibledit
tar xf bibledit-1.0.228.tar.gz -C bibledit --strip-components=1
if [ $? -ne 0 ]
then
  echo Failed to unpack Bibledit
  exit
fi
sleep 4

clear
echo Building Bibledit...
echo cd bibledit
echo ./configure --enable-client --enable-paratext
echo make clean
echo make --jobs=4
sleep 1
cd bibledit
# Remove bits from any older build that might cause crashes in the new build.
find . -name "*.o" -delete
./configure --enable-client --enable-paratext
if [ $? -ne 0 ]
then
  echo Failed to configure Bibledit
  exit
fi
make clean
make --jobs=4
if [ $? -ne 0 ]
then
  echo Failed to build Bibledit
  exit
fi
sleep 4

clear
sleep 1
echo $PASSWORD | sudo -S cp linux/bibledit.sh /usr/bin/bibledit
echo If there were no errors, Bibledit should be working now.
echo Bibledit works best with the Google Chrome browser.
echo Install the browser.
echo To have Bibledit automatically open in Chrome, set Chrome as the default browser.
echo --
echo To start Bibledit, open a terminal, and type:
echo \"bibledit\" 
echo and press Enter.
