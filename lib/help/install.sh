#!/bin/bash

echo Installing Bibledit on Linux.


# Some distro's cannot run $ su.
UNAME=`uname -a`
RUNSU=1;
if [[ $UNAME == *Ubuntu* ]]; then
  RUNSU=0;
fi


# Create a script with commands to run with root privileges.
cat > install2.sh <<'scriptblock'

#!/bin/bash

clear
echo Updating the software sources...
apt-get update

clear
echo Installing the software Bibledit relies on...

apt-get --yes --force-yes install build-essential
dnf --assumeyes install gcc-c++
yum --assumeyes install gcc-c++
zypper --non-interactive install gcc-c++

apt-get --yes --force-yes install git
dnf --assumeyes install git
yum --assumeyes install git
zypper --non-interactive install git

apt-get --yes --force-yes install zip
dnf --assumeyes install zip
yum --assumeyes install zip
zypper --non-interactive install zip

apt-get --yes --force-yes install pkgconf
dnf --assumeyes install pkgconfig
yum --assumeyes install pkgconfig
zypper --non-interactive install pkg-config

apt-get --yes --force-yes install libsqlite3-dev
dnf --assumeyes install sqlite-devel
yum --assumeyes install sqlite-devel
zypper --non-interactive install sqlite3-devel

apt-get --yes --force-yes install libcurl4-openssl-dev
dnf --assumeyes install libcurl-devel
yum --assumeyes install libcurl-devel
zypper --non-interactive install libcurl-devel

apt-get --yes --force-yes install libssl-dev
dnf --assumeyes install openssl-devel
yum --assumeyes install openssl-devel
zypper --non-interactive install libopenssl-devel

apt-get --yes --force-yes install libatspi2.0-dev

zypper --non-interactive install cairo-devel

apt-get --yes --force-yes install libgtk-3-dev
dnf --assumeyes install gtk3-devel
yum --assumeyes install gtk3-devel
zypper --non-interactive install gtk3-devel

# Create the script to start bibledit.
rm -f /usr/bin/bibledit
echo #!/bin/bash >> /usr/bin/bibledit
echo cd  >> /usr/bin/bibledit
echo cd bibledit >> /usr/bin/bibledit
echo ./bibledit >> /usr/bin/bibledit
chmod +x /usr/bin/bibledit

# Act as if the script ran successfully, no matter whether it really did.
exit 0

scriptblock
# This is the end of the script to run with root privileges.


# Run the script with root privileges.
chmod +x install2.sh

# Conditionally run $ su.
if [[ $RUNSU -ne 0 ]]; then
  echo Please provide the password for the root user and press Enter
  su -c ./install2.sh
fi

EXIT_CODE=$?
# If $ su did not run, run $ sudo.
if [[ $RUNSU -eq 0 ]]; then
  EXIT_CODE=1
fi
# If $ su ran, but failed, run $ sudo.
if [ $EXIT_CODE != 0 ]; then

  echo Please provide the password for the administrative user and press Enter:
  sudo ./install2.sh
  EXIT_CODE=$?
  if [ $EXIT_CODE != 0 ]; then
    exit
  fi

fi


# Remove the script with commands to run with root privileges.
rm install2.sh




echo Downloading Bibledit...
cd
rm -f index.html
wget http://bibledit.org/linux -O index.html
if [ $? -ne 0 ]
then
  echo Failed to list tarballs
  exit
fi
cat index.html | grep "bibledit-" | grep -o '<a href=['"'"'"][^"'"'"']*['"'"'"]' | sed -e 's/^<a href=["'"'"']//' -e 's/["'"'"']$//' | tail -n 1 > tarball.txt
rm index.html
TARBALL=`cat tarball.txt`
rm tarball.txt
wget --continue http://bibledit.org/linux/$TARBALL
if [ $? -ne 0 ]
then
  echo Failed to download Bibledit
  exit
fi
sleep 4

echo Unpacking Bibledit in folder bibledit...
mkdir -p bibledit
tar xf $TARBALL -C bibledit --strip-components=1
if [ $? -ne 0 ]
then
  echo Failed to unpack Bibledit
  exit
fi

clear
echo Building Bibledit...
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

echo If there were no errors, Bibledit should be working now.
echo Bibledit works best with the Google Chrome browser.
echo Install the browser.
echo To have Bibledit automatically open in Chrome, set Chrome as the default browser.
echo --
echo To start Bibledit, open a terminal, and type:
echo bibledit
echo and press Enter.
