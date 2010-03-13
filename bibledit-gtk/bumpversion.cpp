/*
** Copyright (©) 2003-2008 Teus Benschop.
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


#include <config.h>
#include <glib.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <string.h>


using namespace std;


int main (int argc, char *argv[])
{
  // Load configure.ac.
  gchar * contents;
  g_file_get_contents ("configure.ac", &contents, NULL, NULL);
  if (contents == NULL) {
    g_warning ("%s", "Can't find configure.ac");
    return 1;
  }
  string text (contents);
  g_free (contents);

  // Look for current version number.
  size_t pos;
  string versionpluscomma = VERSION;
  versionpluscomma.append ("],");
  pos = text.find (versionpluscomma);
  if (pos == string::npos) {
    g_warning ("%s %s", "Can't find current version", VERSION);
    return 1;
  }    

  // Example of a version line:
  // AC_INIT(bibledit, [3.6.54], http://www.nongnu.org/bibledit)  
  
  // Get the bit of text before the version number.
  string textbefore = text.substr (0, pos);
  text.erase (0, pos);
  
  // Get the bit of text after the version number.
  text.erase (0, strlen (VERSION));
  string textafter (text);
  
  // Get the version number.
  text = VERSION;
  
  // Get the bits of the version number.
  vector <string> bits;
  {
    text.append (".");
    size_t position;
    position = text.find (".");
    while (position != string::npos) {
      string word = text.substr (0, position);
      bits.push_back (word);
      text.erase (0, position + 1);
      position = text.find (".");
    }
  }

  // If the version number is like x.x, then don't bump it.
  if (bits.size () < 3) {
    g_warning ("%s %s", "Won't bump", VERSION);
    return 1;
  }

  // Get the minor number and increase it by one.
  unsigned int number = 0;
  istringstream iss (bits[2]);
  iss >> number;
  number++;
  
  // Assemble the new version number.  
  ostringstream oss;
  oss << bits[0] << "." << bits[1] << "." << number;

  // Message.
  g_message ("%s %s", "Bumping the version number to", oss.str ().c_str());
  
  // Assemble the new config.ac.
  text.clear ();
  text.append (textbefore);
  text.append (oss.str ());
  text.append (textafter);
  
  // Write configure.ac
  g_file_set_contents ("configure.ac", text.c_str(), -1, NULL);  
  
  // Write the version number.
  g_file_set_contents ("VERSION", oss.str ().c_str(), -1, NULL);
  
  // Done.
  return 0;
}
