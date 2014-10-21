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
#include <database/sqlite.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <unistd.h>
#include <sys/time.h>


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

  // Number of failed unit tests.  
  error_count = 0;
  
  // Flag for unit tests.
  config_globals_unit_testing = true;

  // Tests for Database_Config_General.
  {
    string ref = "Bible Translation";
    string act = Database_Config_General::getSiteMailName ();
    if (ref != act) error_message ("Database_Config_General::getSiteMailName", ref, act);
  }
  
  {
    string ref = "unittest";
    Database_Config_General::setSiteMailName (ref);
    string actual = Database_Config_General::getSiteMailName ();
    if (ref != actual) error_message ("Database_Config_General::getSiteMailName", ref, actual);
  }
  
  {
    string ref = "";
    string actual = Database_Config_General::getMailStorageSecurity ();
    if (ref != actual) error_message ("Database_Config_General::getMailStorageSecurity", ref, actual);
  }
  
  // Tests for Database_Config_Bible.
  {
    string ref = "";
    string actual = Database_Config_Bible::getViewableByAllUsers ("testbible");
    if (ref != actual) error_message ("Database_Config_Bible::getViewableByAllUsers", ref, actual);
  }
  
  {
    string ref = "1";
    Database_Config_Bible::setViewableByAllUsers ("testbible", ref);
    string actual = Database_Config_Bible::getViewableByAllUsers ("testbible");
    if (ref != actual) error_message ("Database_Config_Bible::getViewableByAllUsers", ref, actual);
  }
  
  // Tests for filter_string.
  // The test shows std::string handles UTF-8 well for simple operations. 
  {
    string ref = "⇊⇦";
    string actual = filter_string_str_replace ("⇖", "", "⇊⇖⇦");
    if (ref != actual) error_message ("filter_string_str_replace", ref, actual);
  }

  // Tests for SQLite.
  {
    sqlite3 * db = database_sqlite_connect ("sqlite");
    if (!db) error_message ("database_sqlite_connect", "pointer", "NULL");
    database_sqlite_exec (db, "CREATE TABLE test (column1 integer, column2 integer, column3 integer);");
    database_sqlite_exec (db, "INSERT INTO test VALUES (123, 456, 789);");
    database_sqlite_exec (db, "INSERT INTO test VALUES (234, 567, 890);");
    database_sqlite_exec (db, "INSERT INTO test VALUES (345, 678, 901);");
    map <string, vector <string> > actual = database_sqlite_query (db, "SELECT column1, column2, column3 FROM test;");
    string ref = "567";
    string actual2 = actual ["column2"] [1]; 
    if (ref != actual2) error_message ("database_sqlite_query", ref, actual2);
    database_sqlite_disconnect (db);
    database_sqlite_disconnect (NULL);
  }
  {
    if (!database_sqlite_healthy ("sqlite")) error_message ("database_sqlite_healthy", "true", "false");
    unlink (database_sqlite_file ("sqlite").c_str());
    if (database_sqlite_healthy ("sqlite")) error_message ("database_sqlite_healthy", "false", "true");
  }
  {
    string ref = "He''s";
    string actual = database_sqlite_no_sql_injection ("He's");
    if (ref != actual) error_message ("database_sqlite_no_sql_injection", ref, actual);
  }

  // Tests for Database_Logs.
  {
    refresh_sandbox ();
    // Log some items.
    // Temporarily disable output to stdout to avoid clutter there.
    config_globals_unit_testing = false;
    Database_Logs::log ("description1", 2);
    Database_Logs::log ("description2", 3);
    Database_Logs::log ("description3", 4);
    config_globals_unit_testing = true;
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    database_logs.checkup ();
    // Move the items from the filesystem into the SQLite database.
    database_logs.rotate ();
    // Get the items from the SQLite database.
    int lastsecond = 0;
    vector <string> result = database_logs.get (0, lastsecond);
    if (result.size () != 3) error_message ("Database_Logs::get", "Size should be 3", filter_string_convert_to_string (result.size ()));
  }
  
  {
    // Check the database: It should recreate the database and then create one entry in the Journal.
    // This entry is proof that it recreated the database.
    refresh_sandbox ();
    Database_Logs database_logs = Database_Logs ();
    config_globals_unit_testing = false;
    database_logs.checkup ();
    config_globals_unit_testing = true;
    int lastsecond = 1111111111;
    vector <string> result = database_logs.get (0, lastsecond);
    if (result.size () != 1) error_message ("Database_Logs::get", "Size should be 1", filter_string_convert_to_string (result.size ()));
  }
  
  {
    refresh_sandbox ();
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int now = tv.tv_sec;
    int min1days = now - 86400 - 10;
    int min2days = min1days - 86400;
    int min3days = min2days - 86400;
    int min4days = min3days - 86400;
    int min5days = min4days - 86400;
    int min6days = min5days - 86400;
    int lastsecond = 0;
    vector <string> result;
    config_globals_unit_testing = false;

    // Log entry for 6 days ago.
    Database_Logs::log ("Six days ago");
    database_logs.rotate ();
    database_logs.update (now, min6days);
    lastsecond = 0;
    result = database_logs.get (6, lastsecond);
    if (result.size () != 1) error_message ("Database_Logs::get", "Six days ago: Size should be 1", filter_string_convert_to_string (result.size ()));
    lastsecond = 0;
    result = database_logs.get (5, lastsecond);
    if (result.size () != 0) error_message ("Database_Logs::get", "Six days ago: Size should be 0", filter_string_convert_to_string (result.size ()));
    // Rotate that entry away.
    database_logs.rotate ();
    lastsecond = 0;
    result = database_logs.get (6, lastsecond);
    if (result.size () != 0) error_message ("Database_Logs::get", "Rotate six days away: Size should be 0", filter_string_convert_to_string (result.size ()));

    // Log entry for 2 days, 1 day ago, and now.
    Database_Logs::log ("Two days ago");
    database_logs.rotate ();
    database_logs.update (now, min2days);
    Database_Logs::log ("One day ago");
    database_logs.rotate ();
    database_logs.update (now, min1days);
    Database_Logs::log ("Now");
    lastsecond = 0;
    result = database_logs.get (2, lastsecond);
    if (result.size () != 1) error_message ("Database_Logs::get", "Two days ago: Size should be 1", filter_string_convert_to_string (result.size ()));
    lastsecond = 0;
    // Gets it from the filesystem, not the database, because this last entry was not yet rotated.
    result = database_logs.get (0, lastsecond);
    if (result.size () != 1) error_message ("Database_Logs::get", "Now: Size should be 1", filter_string_convert_to_string (result.size ()));
    // The "lastsecond" variable, test it.
    if ((lastsecond < now ) || (lastsecond > now + 1)) error_message ("Database_Logs::get lastsecond", now, lastsecond);

    // Done.
    config_globals_unit_testing = true;
  }

  {
    // Test huge journal entry.
    refresh_sandbox ();
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    config_globals_unit_testing = false;
    string huge (10000, 'x');
    Database_Logs::log (huge);
    database_logs.rotate ();
    int i = 0;
    vector <string> result = database_logs.get (0, i);
    if (result.size () == 1) {
      string s = result[0];
      size_t pos = s.find ("This entry was too large and has been truncated: 10013 bytes");
      if (pos == string::npos) {
        error_message ("Database_Logs: test huge entry", "Should be truncated", s);
      }
    } else {
      error_message ("Database_Logs: test huge entry", "Size should be 1", filter_string_convert_to_string (result.size ()));
    }
    config_globals_unit_testing = true;
  }
  
  {
    // Test the getNext function of the Journal.
    refresh_sandbox ();
    config_globals_unit_testing = false;
    Database_Logs::log ("description");
    config_globals_unit_testing = true;
    Database_Logs database_logs = Database_Logs ();
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int second = tv.tv_sec;
    string filename = filter_string_convert_to_string (second) + "00000000";
    // First time: getNext gets the logged entry.
    string s;
    s = database_logs.getNext (filename);
    if (s == "") error_message ("Database_Logs::getNext", "...description", s);
    // Since variable "filename" is updated and set to the last filename,
    // next time getNext gets nothing.
    s = database_logs.getNext (filename);
    if (s != "") error_message ("Database_Logs::getNext", "", s);
  }

  // Tests for Filter_Roles.
  {
    int ref = 3;
    int act = Filter_Roles::consultant ();
    if (ref != act) error_message ("Filter_Roles::consultant", ref, act);
  }
  {
    int ref = 1;
    int act = Filter_Roles::lowest ();
    if (ref != act) error_message ("Filter_Roles::lowest", ref, act);
  }


  
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;
}


