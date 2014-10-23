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
#include <filter/md5.h>
#include <unistd.h>
#include <sys/time.h>
#include <database/users.h>


using namespace std;


// Puts a fresh and clean copy of Bibledit into the sandbox in the testing directory.
void refresh_sandbox (bool displayjournal)
{
  // Display any old journal entries.
  if (displayjournal) {
    string directory = filter_url_create_path (testing_directory, "logbook");
    vector <string> files = filter_url_scandir (directory);
    for (unsigned int i = 0; i < files.size (); i++) {
      string contents = filter_url_file_get_contents (filter_url_create_path (directory, files [i]));
      cout << contents << endl;
    }
  }
  
  // Refresh.
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


void evaluate (string function, string desired, string actual)
{
  if (desired != actual) error_message (function, desired, actual);
}


void evaluate (string function, int desired, int actual)
{
  if (desired != actual) error_message (function, filter_string_convert_to_string (desired), filter_string_convert_to_string (actual));
}


void evaluate (string function, bool desired, bool actual)
{
  if (desired != actual) error_message (function, filter_string_convert_to_string (desired), filter_string_convert_to_string (actual));
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
  refresh_sandbox (true);
  config_globals_document_root = testing_directory;

  // Number of failed unit tests.  
  error_count = 0;
  
  // Flag for unit tests.
  config_globals_unit_testing = true;

  // Tests for Database_Config_General.
  {
    evaluate ("Database_Config_General::getSiteMailName", "Bible Translation", Database_Config_General::getSiteMailName ());
  }
  {
    string ref = "unittest";
    Database_Config_General::setSiteMailName (ref);
    evaluate ("Database_Config_General::getSiteMailName", ref, Database_Config_General::getSiteMailName ());
  }
  {
    evaluate ("Database_Config_General::getMailStorageSecurity", "", Database_Config_General::getMailStorageSecurity ());
  }
  
  // Tests for Database_Config_Bible.
  {
    evaluate ("Database_Config_Bible::getViewableByAllUsers", "", Database_Config_Bible::getViewableByAllUsers ("testbible"));
  }
  {
    string ref = "1";
    Database_Config_Bible::setViewableByAllUsers ("testbible", ref);
    evaluate ("Database_Config_Bible::getViewableByAllUsers", ref, Database_Config_Bible::getViewableByAllUsers ("testbible"));
  }
  
  // Tests for filter_string.
  // The test shows std::string handles UTF-8 well for simple operations. 
  {
    evaluate ("filter_string_str_replace", "⇊⇦", filter_string_str_replace ("⇖", "", "⇊⇖⇦"));
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
    evaluate ("database_sqlite_query", "567", actual ["column2"] [1]);
    database_sqlite_disconnect (db);
    database_sqlite_disconnect (NULL);

    evaluate ("database_sqlite_healthy", true, database_sqlite_healthy ("sqlite"));
    unlink (database_sqlite_file ("sqlite").c_str());
    evaluate ("database_sqlite_healthy", false, database_sqlite_healthy ("sqlite"));

    evaluate ("database_sqlite_no_sql_injection", "He''s", database_sqlite_no_sql_injection ("He's"));
  }

  // Tests for Database_Logs.
  {
    refresh_sandbox (true);
    // Log some items.
    // Temporarily disable output to stdout to avoid clutter there.
    Database_Logs::log ("description1", 2);
    Database_Logs::log ("description2", 3);
    Database_Logs::log ("description3", 4);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    database_logs.checkup ();
    // Move the items from the filesystem into the SQLite database.
    database_logs.rotate ();
    // Get the items from the SQLite database.
    int lastsecond = 0;
    vector <string> result = database_logs.get (0, lastsecond);
    evaluate ("Database_Logs::get", 3, result.size ());
    refresh_sandbox (false);
  }
  {
    // Check the database: It should recreate the database and then create one entry in the Journal.
    // This entry is proof that it recreated the database.
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.checkup ();
    int lastsecond = 1111111111;
    vector <string> result = database_logs.get (0, lastsecond);
    evaluate ("Database_Logs::get", 1, result.size ());
    refresh_sandbox (false);
  }
  {
    refresh_sandbox (true);
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

    // Log entry for 6 days ago.
    Database_Logs::log ("Six days ago");
    database_logs.rotate ();
    database_logs.update (now, min6days);
    lastsecond = 0;
    result = database_logs.get (6, lastsecond);
    evaluate ("Database_Logs::get: Six days ago: Size should be 1", 1, result.size ());
    lastsecond = 0;
    result = database_logs.get (5, lastsecond);
    evaluate ("Database_Logs::get: Five days ago: Size should be 0", 0, result.size ());
    // Rotate that entry away.
    database_logs.rotate ();
    lastsecond = 0;
    result = database_logs.get (6, lastsecond);
    evaluate ("Database_Logs::get: Rotate six days away: Size should be 0", 0, result.size ());

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
    evaluate ("Database_Logs::get: Two days ago: Size should be 1", 1, result.size ());
    // Gets it from the filesystem, not the database, because this last entry was not yet rotated.
    lastsecond = 0;
    result = database_logs.get (0, lastsecond);
    evaluate ("Database_Logs::get: Now: Size should be 1", 1, result.size ());
    // The "lastsecond" variable, test it.
    if ((lastsecond < now ) || (lastsecond > now + 1)) evaluate ("Database_Logs::get lastsecond", now, lastsecond);
    refresh_sandbox (false);
  }
  {
    // Test huge journal entry.
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
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
      evaluate ("Database_Logs: test huge entry", 1, result.size ());
    }
    refresh_sandbox (false);
  }
  {
    // Test the getNext function of the Journal.
    refresh_sandbox (true);
    Database_Logs::log ("description");
    Database_Logs database_logs = Database_Logs ();
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int second = tv.tv_sec;
    string filename = filter_string_convert_to_string (second) + "00000000";
    // First time: getNext gets the logged entry.
    string s;
    s = database_logs.getNext (filename);
    if (s == "") evaluate ("Database_Logs::getNext", "...description", s);
    // Since variable "filename" is updated and set to the last filename,
    // next time function getNext gets nothing.
    s = database_logs.getNext (filename);
    evaluate ("Database_Logs::getNext", "", s);
    refresh_sandbox (false);
  }

  // Tests for Filter_Roles.
  {
    evaluate ("Filter_Roles::consultant", 3, Filter_Roles::consultant ());
  }
  {
    evaluate ("Filter_Roles::lowest", 1, Filter_Roles::lowest ());
  }

  // Tests for the C++ md5 function as compared to PHP's version.
  {
    evaluate ("md5", "1f3870be274f6c49b3e31a0c6728957f", md5 ("apple"));
  }
  
  // Tests for Database_Users.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username = "unit test";
    string password = "pazz";
    int level = 10;
    string email = "email@site.nl";

    database_users.optimize ();
    database_users.trim ();
    
    database_users.addNewUser (username, password, level, email);

    evaluate ("Database_Users::matchUsernamePassword", true, database_users.matchUsernamePassword (username, password));
    evaluate ("Database_Users::matchUsernamePassword (wrong password)", false, database_users.matchUsernamePassword (username, "wrong password"));

    evaluate ("Database_Users::matchEmailPassword", true, database_users.matchEmailPassword (email, password));
    evaluate ("Database_Users::matchEmailPassword (wrong password)", false, database_users.matchEmailPassword (email, "wrong password"));

    string ref = "INSERT INTO users (username, password, level, email) VALUES ('unit test', '014877e71841e82d44ce524d66dcc732', 10, 'email@site.nl');";
    string act = database_users.addNewUserQuery (username, password, level, email);
    evaluate ("Database_Users::addNewUserQuery", ref, act);

    evaluate ("Database_Users::getEmailToUser", username, database_users.getEmailToUser (email));
    evaluate ("Database_Users::getEmailToUser (wrong email)", "", database_users.getEmailToUser ("wrong email"));

    evaluate ("Database_Users::getUserToEmail", email, database_users.getUserToEmail (username));
    evaluate ("Database_Users::getUserToEmail (wrong username)", "", database_users.getUserToEmail ("wrong username"));

    evaluate ("Database_Users::usernameExists", true, database_users.usernameExists (username));
    evaluate ("Database_Users::usernameExists (invalid username)", false, database_users.usernameExists ("invalid username"));

    evaluate ("Database_Users::emailExists", true, database_users.emailExists (email));
    evaluate ("Database_Users::emailExists (invalid email)", false, database_users.emailExists ("invalid email"));

    evaluate ("Database_Users::getUserLevel", level, database_users.getUserLevel (username));
    evaluate ("Database_Users::getUserLevel (invalid user)", Filter_Roles::guest (), database_users.getUserLevel ("invalid username"));

    level = 7;
    database_users.updateUserLevel (username, level);
    evaluate ("Database_Users::getUserLevel after updateUserLevel", level, database_users.getUserLevel (username));

    database_users.removeUser (username);
    evaluate ("Database_Users::usernameExists after removeUser", false, database_users.usernameExists (username));
    
    evaluate ("Database_Users::updateEmailQuery", "UPDATE users SET email = 'email@site.nl' WHERE username = 'unit test';", database_users.updateEmailQuery (username, email));
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test1";
    string username2 = "unit test2";
    string password = "pazz";
    int level = Filter_Roles::admin();
    string email = "email@site";

    database_users.addNewUser (username1, password, level, email);
    vector <string> admins = database_users.getAdministrators ();
    evaluate ("Database_Users::getAdministrators one user", 1, admins.size());
    if (!admins.empty()) evaluate ("Database_Users::getAdministrators user", username1, admins [0]);
    
    database_users.addNewUser (username2, password, level, email);
    admins = database_users.getAdministrators ();
    evaluate ("Database_Users::getAdministrators two users", 2, admins.size());
    
    email = "new@email.address";
    database_users.updateUserEmail (username1, email);
    evaluate ("Database_Users::updateUserEmail", email, database_users.getUserToEmail (username1));
    
    vector <string> users = database_users.getUsers ();
    evaluate ("Database_Users::getUsers", 2, users.size());
    
    evaluate ("Database_Users::getmd5", md5 (password), database_users.getmd5 (username1));
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username = "unit test";
    string password = "pazz";
    string email = "email@site";
    string address = "192.168.1.0";
    string agent = "Browser's user agent";
    string fingerprint = "ԴԵԶԸ";

    database_users.setTokens (username, address, agent, fingerprint);
    evaluate ("Database_Users::getUsername 1", username, database_users.getUsername (address, agent, fingerprint));
    database_users.removeTokens (username);
    evaluate ("Database_Users::getUsername 2", "", database_users.getUsername (address, agent, fingerprint));

    evaluate ("Database_Users::getTimestamp 0", 0, database_users.getTimestamp (username));
    struct timeval tv;
    gettimeofday (&tv, NULL);
    database_users.pingTimestamp (username);
    int timestamp = database_users.getTimestamp (username);
    if ((timestamp != tv.tv_sec) && (timestamp != tv.tv_sec + 1)) evaluate ("Database_Users::getTimestamp time", tv.tv_sec, timestamp);
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test 1";
    string username2 = "unit test 2";
    string bible1 = "bible one";
    string bible2 = "bible two";
    
    vector <string> teams = database_users.getTeams ();
    evaluate ("Database_Users::getTeams 0", 0, teams.size());
    
    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate ("Database_Users::getTeams 1", 1, teams.size());

    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate ("Database_Users::getTeams 2", 1, teams.size());
    if (!teams.empty ()) evaluate ("Database_Users::getTeams name", bible1, teams [0]);
    
    database_users.grantAccess2Bible (username1, bible2);
    teams = database_users.getTeams ();
    evaluate ("Database_Users::getTeams 3", 2, teams.size());
    
    vector <string> bibles = database_users.getBibles4User (username1);
    evaluate ("Database_Users::getBibles4User 1", 2, bibles.size());
    if (!bibles.empty ()) evaluate ("Database_Users::getBibles4User name", bible1, bibles [0]);
    
    database_users.revokeAccess2Bible (username1, bible1);
    bibles = database_users.getBibles4User (username1);
    evaluate ("Database_Users::getBibles4User 2", 1, bibles.size());

    database_users.grantAccess2Bible (username2, bible2);
    vector <string> users = database_users.getUsers4Bible (bible2);
    evaluate ("Database_Users::getUsers4Bible 1", 2, users.size());
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();

    string username1 = "unit test 1";
    string username2 = "unit test 2";
    string bible1 = "bible one";
    string bible2 = "bible two";
    
    // No teams: Any user has access to any Bible.
    evaluate ("Database_Users::hasAccess2Bible 1", true, database_users.hasAccess2Bible (username1, bible1));
    
    // One team: Any user has access to any Bible.
    database_users.grantAccess2Bible (username1, bible1);
    evaluate ("Database_Users::hasAccess2Bible 2", true, database_users.hasAccess2Bible (username1, bible1));
    evaluate ("Database_Users::hasAccess2Bible 3", true, database_users.hasAccess2Bible (username2, bible2));

    // Two teams: User access control applies.
    database_users.grantAccess2Bible (username1, bible1);
    database_users.grantAccess2Bible (username1, bible2);
    database_users.grantAccess2Bible (username2, bible1);
    evaluate ("Database_Users::hasAccess2Bible 4", true, database_users.hasAccess2Bible (username1, bible1));
    evaluate ("Database_Users::hasAccess2Bible 5", true, database_users.hasAccess2Bible (username1, bible2));
    evaluate ("Database_Users::hasAccess2Bible 6", true, database_users.hasAccess2Bible (username2, bible1));
    evaluate ("Database_Users::hasAccess2Bible 7", false, database_users.hasAccess2Bible (username2, bible2));
    
    // Admin has access to any Bible.
    database_users.addNewUser (username2, "", Filter_Roles::admin (), "");
    evaluate ("Database_Users::hasAccess2Bible 8", true, database_users.hasAccess2Bible (username2, bible2));
    
    // Read only access for known user.
    evaluate ("Database_Users::hasReadOnlyAccess2Bible 1", false, database_users.hasReadOnlyAccess2Bible (username1, bible1));
    database_users.setReadOnlyAccess2Bible (username1, bible1, true);
    evaluate ("Database_Users::hasReadOnlyAccess2Bible 2", true, database_users.hasReadOnlyAccess2Bible (username1, bible1));
    
    // No read-only access for unknown user.
    evaluate ("Database_Users::hasReadOnlyAccess2Bible 3", false, database_users.hasReadOnlyAccess2Bible ("unknown", bible1));
  }
  







  // Possible journal entries.
  refresh_sandbox (true);
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;
}


