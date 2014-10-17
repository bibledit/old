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
#include <config/globals.h>


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
}


int main (int argc, char **argv) 
{
  cout << "Running Bibledit unittests" << endl;

  // No compile warnings.
  if (argc) {};
  if (argv[0]) {};
  
  // Directory where the unit tests will run.
  testing_directory = "/tmp/bibledit-unittests";  
  mkdir (testing_directory.c_str(), 0777);
  refresh_sandbox ();
  config_globals_document_root = testing_directory;

  // Variables for general use.  
  string s1, s2;
  vector <string> vs1, vs2;
  
  // Tests for Database_Config_General.
  Database_Config_General database_config_general = Database_Config_General ();

  s1 = "Bible Translation";
  s2 = database_config_general.getSiteMailName ();
  if (s1 != s2) error_message ("Database_Config_General::getSiteMailName", s1, s2);
  s1 = "unittest";
  database_config_general.setSiteMailName (s1);
  s2 = database_config_general.getSiteMailName ();
  if (s1 != s2) error_message ("Database_Config_General::getSiteMailName", s1, s2);

  
  
  return 0;
}


