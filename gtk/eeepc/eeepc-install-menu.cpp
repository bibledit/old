/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
#include <glib.h>


using namespace std;


// This utilitiy installs the menu in the Asus Eee PC.


int main(int argc, char *argv[])
{
  // The directory where the menu data is stored.
  string directory = "/opt/xandros/share/AsusLauncher";
  
  // If the directory isn't there, we're not on the Asus Eee PC: bail out.
  if (!g_file_test (directory.c_str(), G_FILE_TEST_IS_DIR)) {
    cout << "Not installing on the Asus Eee PC." << endl;
    return 0;
  }

  // Copy the icons to the directory.
  string command = "cp *.png " + directory;
  cout << command << endl; 
  if (system (command.c_str()));
  
  // Filename of the original simplegui.rc file.
  string originalfile;
  gchar *name = g_build_filename (directory.c_str(), "simpleui.rc", NULL);
  originalfile = name;
  g_free (name);

  // Directory of the user's menu.
  string userdir;
  name = g_build_filename ("/home", "user", ".AsusLauncher", NULL);
  userdir = name;
  g_free (name);
  
  // Create directory if it does not yet exist.
  command = "mkdir -p " + userdir;
  cout << command << endl; 
  if (system (command.c_str ()));
  
  // Filename of the user's simplegui.rc file.
  string userfile;
  name = g_build_filename (userdir.c_str(), "simpleui.rc", NULL);
  userfile = name;
  g_free (name);

  // If the user's .rc file is not yet there, copy it from the original.
  if (!g_file_test (userfile.c_str(), G_FILE_TEST_IS_REGULAR)) {
    command = "cp " + originalfile + " " + userfile;
    cout << command << endl; 
    if (system (command.c_str()));
  }

  // Read the menu rc file.
  string content;
  gchar * contents;
  g_file_get_contents (userfile.c_str(), &contents, NULL, NULL);
  if (contents) {
    content = contents;
  }

  /*
  The menu entry is the following text:
  
<parcel simplecat="Work" extraargs="/usr/bin/bibledit" 
	icon="bibledit_norm.png" 
	selected_icon="bibledit_hi.png" >
	<name lang="en">Bibledit</name>
</parcel>

  This comes just before the line that reads:
  
</simpleui>
  
  */

  // If the menu is already in, ok, bail out.
  if (content.find ("bibledit") != string::npos) {
    cout << "Menu already there. Done" << endl;
    return 0;
  }

  // Insert the menu.
  size_t pos = content.find ("</simpleui>");
  if (pos == string::npos) {
    cout << "Cannot find where to insert the menu" << endl;
    return 1;
  }
  string menu = "<parcel simplecat=\"Work\" extraargs=\"/usr/bin/bibledit\"\n" 
                "        icon=\"bibledit_norm.png\"\n"
                "        selected_icon=\"bibledit_hi.png\" >\n"
                "        <name lang=\"en\">Bibledit</name>\n"
                "</parcel>\n"
                "\n"
                "\n";
  content.insert (pos, menu);
  
  // Write data.
  g_file_set_contents (userfile.c_str(), content.c_str(), -1, NULL);
  
  // Done.
  return 0;
}
