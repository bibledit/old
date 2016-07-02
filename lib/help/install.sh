#!/bin/bash


# Some distro's cannot run $ su.
UNAME=`uname -a`
echo -n "Installing Bibledit on "
echo $UNAME
RUNSU=1;
if [ "$UNAME"="*Ubuntu*" ]; then
  RUNSU=0;
fi


# Deal with command line arguments.
DRYECHO=""
if [ $# -ne 0 ]; then
  if [ "$1" = "-dry" ]; then
    DRYECHO="echo"
  else
    echo Pass no arguments to install Bibledit.
    echo Pass argument -dry to do a dry run.
    exit 0
  fi
fi


# Create a script with commands to run with root privileges.
cat > install2.sh <<'scriptblock'

#!/bin/bash

DRYECHO=$1

clear
echo Updating the software sources...
which apt-get > /dev/null
if [ $? -eq 0 ]; then
$DRYECHO apt-get update
fi

echo Installing the software Bibledit relies on...

which apt-get > /dev/null
if [ $? -eq 0 ]
then
$DRYECHO apt-get --yes --force-yes install build-essential
$DRYECHO apt-get --yes --force-yes install git
$DRYECHO apt-get --yes --force-yes install zip
$DRYECHO apt-get --yes --force-yes install pkgconf
$DRYECHO apt-get --yes --force-yes install libsqlite3-dev
$DRYECHO apt-get --yes --force-yes install libcurl4-openssl-dev
$DRYECHO apt-get --yes --force-yes install libssl-dev
$DRYECHO apt-get --yes --force-yes install libatspi2.0-dev
$DRYECHO apt-get --yes --force-yes install libgtk-3-dev
fi

which dnf > /dev/null
if [ $? -eq 0 ]
then
$DRYECHO dnf --assumeyes install gcc-c++
$DRYECHO dnf --assumeyes install git
$DRYECHO dnf --assumeyes install zip
$DRYECHO dnf --assumeyes install pkgconfig
$DRYECHO dnf --assumeyes install sqlite-devel
$DRYECHO dnf --assumeyes install libcurl-devel
$DRYECHO dnf --assumeyes install openssl-devel
$DRYECHO dnf --assumeyes install gtk3-devel
fi

which yum > /dev/null
if [ $? -eq 0 ]
then
$DRYECHO yum --assumeyes install gcc-c++
$DRYECHO yum --assumeyes install git
$DRYECHO yum --assumeyes install zip
$DRYECHO yum --assumeyes install pkgconfig
$DRYECHO yum --assumeyes install sqlite-devel
$DRYECHO yum --assumeyes install libcurl-devel
$DRYECHO yum --assumeyes install openssl-devel
$DRYECHO yum --assumeyes install gtk3-devel
fi

which zypper > /dev/null
if [ $? -eq 0 ]
then
$DRYECHO zypper --non-interactive install gcc-c++
$DRYECHO zypper --non-interactive install git
$DRYECHO zypper --non-interactive install zip
$DRYECHO zypper --non-interactive install pkg-config
$DRYECHO zypper --non-interactive install sqlite3-devel
$DRYECHO zypper --non-interactive install libcurl-devel
$DRYECHO zypper --non-interactive install libopenssl-devel
$DRYECHO zypper --non-interactive install cairo-devel
$DRYECHO zypper --non-interactive install gtk3-devel
fi

# Create the script to start bibledit.
rm -f /usr/bin/bibledit
echo #!/bin/bash >> /usr/bin/bibledit
echo cd  >> /usr/bin/bibledit
echo cd bibledit >> /usr/bin/bibledit
echo './bibledit 2>&1 | grep -v WARNING | tr -d 012' >> /usr/bin/bibledit
chmod +x /usr/bin/bibledit

# Act as if the script ran successfully, no matter whether it really did.
exit 0

scriptblock
# This is the end of the script to run with root privileges.


# Run the script with root privileges.
chmod +x install2.sh

# Conditionally run $ su.
if [ $RUNSU -ne 0 ]; then
  echo Please provide the password for the root user and press Enter
  su -c ./install2.sh $DRYECHO
fi

EXIT_CODE=$?
# If $ su did not run, run $ sudo.
if [ $RUNSU -eq 0 ]; then
  EXIT_CODE=1
fi
# If $ su ran, but failed, run $ sudo.
if [ $EXIT_CODE != 0 ]; then

  echo Please provide the password for the administrative user and press Enter:
  sudo ./install2.sh $DRYECHO
  EXIT_CODE=$?
  if [ $EXIT_CODE != 0 ]; then
    exit
  fi

fi


# Remove the script with commands to run with root privileges.
rm install2.sh


cd
rm -f index.html
wget http://bibledit.org/linux -q -O index.html
if [ $? -ne 0 ]
then
  echo Failed to list tarballs
  exit
fi
cat index.html | grep "bibledit-" | grep -o '<a href=['"'"'"][^"'"'"']*['"'"'"]' | sed -e 's/^<a href=["'"'"']//' -e 's/["'"'"']$//' | tail -n 1 > tarball.txt
rm index.html
TARBALL=`cat tarball.txt`
rm tarball.txt
$DRYECHO wget --continue http://bibledit.org/linux/$TARBALL
if [ $? -ne 0 ]
then
  echo Failed to download Bibledit
  exit
fi

$DRYECHO mkdir -p bibledit
$DRYECHO tar xf $TARBALL -C bibledit --strip-components=1
if [ $? -ne 0 ]
then
  echo Failed to unpack Bibledit
  exit
fi

$DRYECHO cd bibledit
# Remove bits from any older build that might cause crashes in the new build.
find . -name "*.o" -delete
$DRYECHO ./configure --enable-paratext --enable-linux
if [ $? -ne 0 ]
then
  echo Failed to configure Bibledit
  exit
fi
$DRYECHO make clean
$DRYECHO make --jobs=4
if [ $? -ne 0 ]
then
  echo Failed to build Bibledit
  exit
fi

echo If there were no errors, Bibledit should be working now.
echo Bibledit works best with the Google Chrome browser.
echo Install the browser.
echo To automatically open Bibledit in Chrome, set Chrome as the default browser.
echo --
echo To start Bibledit, open a terminal, and type:
echo bibledit
echo and press Enter.
