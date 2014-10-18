/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <iostream>
#include <cstdlib>
#include <library/bibledit.h>
#include <sys/stat.h>
#include <unittests/unittest.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>


using namespace std;


// Puts a fresh and clean copy of Bibledit into the sandbox in the testing directory.
void refresh_sandbox ()
{
  string command = "rsync . -a --delete " + testing_directory;
  int status = system (command.c_str());
  if (status != 0) {
    cout << "Error while running " + command << endl;
    exit (status);
  }
}


void error_message (string function, string desired, string actual)
{
  cout << "Function:       " << function << endl;
  cout << "Desired result: " << desired << endl;
  cout << "Actual result:  " << actual << endl;
  cout << endl;
  error_count++;
}


void error_message (string function, int desired, int actual)
{
  string s1 = filter_string_convert_to_string (desired);
  string s2 = filter_string_convert_to_string (actual);
  error_message (function, s1, s2);
}


int main (int argc, char **argv) 
{
  cout << "Running unittests" << endl;

  // No compile warnings.
  if (argc) {};
  if (argv[0]) {};
  
  // Directory where the unit tests will run.
  testing_directory = "/tmp/bibledit-unittests";  
  filter_url_mkdir (testing_directory);
  refresh_sandbox ();
  config_globals_document_root = testing_directory;
  
  error_count = 0;

  // Variables for general use.  
  string s1, s2;
  int i1, i2;
  vector <string> vs1, vs2;
 
  // Tests for Database_Config_General.
  s1 = "Bible Translation";
  s2 = Database_Config_General::getSiteMailName ();
  if (s1 != s2) error_message ("Database_Config_General::getSiteMailName", s1, s2);
  s1 = "unittest";
  Database_Config_General::setSiteMailName (s1);
  s2 = Database_Config_General::getSiteMailName ();
  if (s1 != s2) error_message ("Database_Config_General::getSiteMailName", s1, s2);

  s1 = "";
  s2 = Database_Config_General::getMailStorageSecurity ();
  if (s1 != s2) error_message ("Database_Config_General::getMailStorageSecurity", s1, s2);
  
  // Tests for Database_Config_Bible.
  s1 = "";
  s2 = Database_Config_Bible::getViewableByAllUsers ("testbible");
  if (s1 != s2) error_message ("Database_Config_Bible::getViewableByAllUsers", s1, s2);
  s1 = "1";
  Database_Config_Bible::setViewableByAllUsers ("testbible", s1);
  s2 = Database_Config_Bible::getViewableByAllUsers ("testbible");
  if (s1 != s2) error_message ("Database_Config_Bible::getViewableByAllUsers", s1, s2);
  
  // Tests for filter_string.
  s1 = "⇊⇦";
  s2 = filter_string_str_replace ("⇖", "", "⇊⇖⇦");
  if (s1 != s2) error_message ("filter_string_str_replace", s1, s2);

  // Tests for Database_Logs.
  Database_Logs::log ("description", 2);
  
  // Tests for Filter_Roles.
  i1 = 3;
  i2 = Filter_Roles::consultant ();
  if (i1 != i2) error_message ("Filter_Roles::consultant", i1, i2);
  i1 = 1;
  i2 = Filter_Roles::lowest ();
  if (i1 != i2) error_message ("Filter_Roles::lowest", i1, i2);
  
  
  
  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;
  return 0;
}


