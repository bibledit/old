/*
** Copyright (C) 2003-2006 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>
#include <vector>
#include <set>


using namespace std;


/*
This utilitiy helps in installing Bibledit on the OLPC.

It gathers all libraries that Bibledit and its dependencies use,
and lists them out.

It will create a gzipped tarball that contains all libraries Bibledit needs.
Also Bibledit itself will be put in it.

If this tarball is unpacked in the OLPC, then Bibledit will work there.
*/


int main(int argc, char *argv[])
{
  // Usage info.
  if (argc == 1) {
    cout << "Bibledit for OLPC Librarian " << PACKAGE_VERSION << endl;
    cout << "Produces a tarball with all libraries Bibledit depends on." << endl;
    cout << "Usage:" << endl;
    cout << "bibledit-olpc-libraries tarball" << endl;
    cout << "tarball is the name of the gzipped tarball that will be created, with all" << endl;
    cout << "libraries that need to be transferred to the OLPC image" << endl;
    return 0;
  }
  // Create the list of files to include in the tarball.  
  vector <string> tarfiles;
  tarfiles.push_back ("/usr/share/bibledit/*");
  tarfiles.push_back ("/usr/bin/bibledit*");
  tarfiles.push_back ("/usr/lib/libsqlite*");
  tarfiles.push_back ("/usr/lib/chrome");
  tarfiles.push_back ("/usr/lib/components");
  tarfiles.push_back ("/usr/lib/defaults");
  tarfiles.push_back ("/usr/lib/greprefs");
  tarfiles.push_back ("/usr/lib/icons");
  tarfiles.push_back ("/usr/lib/install");
  tarfiles.push_back ("/usr/lib/libgtkembedmoz.so");
  tarfiles.push_back ("/usr/lib/libmozjs.so");
  tarfiles.push_back ("/usr/lib/libnspr4.so");
  tarfiles.push_back ("/usr/lib/libnss3.so");
  tarfiles.push_back ("/usr/lib/libnssckbi.so");
  tarfiles.push_back ("/usr/lib/libplc4.so");
  tarfiles.push_back ("/usr/lib/libplds4.so");
  tarfiles.push_back ("/usr/lib/libsmime3.so");
  tarfiles.push_back ("/usr/lib/libsoftokn3.chk");
  tarfiles.push_back ("/usr/lib/libsoftokn3.so");
  tarfiles.push_back ("/usr/lib/libssl3.so");
  tarfiles.push_back ("/usr/lib/libxpcom.so");
  tarfiles.push_back ("/usr/lib/libxul.so");
  tarfiles.push_back ("/usr/lib/mozilla-installer-bin");
  tarfiles.push_back ("/usr/lib/mozilla-xremote-client");
  tarfiles.push_back ("/usr/lib/plugins");
  tarfiles.push_back ("/usr/lib/regxpcom");
  tarfiles.push_back ("/usr/lib/res");
  tarfiles.push_back ("/usr/lib/run-mozilla.sh");
  tarfiles.push_back ("/usr/lib/TestGtkEmbed");
  tarfiles.push_back ("/usr/lib/updater");
  tarfiles.push_back ("/usr/lib/xpcshell");
  tarfiles.push_back ("/usr/lib/xpicleanup");
  tarfiles.push_back ("/usr/lib/xpidl");
  tarfiles.push_back ("/usr/lib/xpt_dump");
  tarfiles.push_back ("/usr/lib/xpt_link");
  tarfiles.push_back ("/usr/lib/xulrunner-bin");
  tarfiles.push_back ("/usr/lib/xulrunner-stub");
  // Create tarball and gzip it.
  string tarball = argv[1];
  tarball.append (".tar");
  unlink (tarball.c_str());
  string command = "tar -cP";
  for (unsigned int i = 0; i < tarfiles.size(); i++) {
    command.append (" " + tarfiles[i]);
  }
  command.append (" > " + tarball);
  system (command.c_str());
  string gzipped_tarball = tarball + ".gz";
  unlink (gzipped_tarball.c_str());
  command = "gzip " + tarball;
  system (command.c_str());  
  cout << "Created gzipped tarball " << gzipped_tarball << endl;
  return 0;
}
