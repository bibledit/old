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


#include <config/libraries.h>
#include <library/bibledit.h>
#include <unittests/unittest.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/config/user.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <session/logic.h>


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


void evaluate (string function, vector <string> desired, vector <string> actual)
{
  if (desired.size() != actual.size ()) {
    error_message (function + " size mismatch", filter_string_convert_to_string ((int)desired.size ()), filter_string_convert_to_string ((int)actual.size()));
    return;
  }
  for (unsigned int i = 0; i < desired.size (); i++) {
    if (desired[i] != actual[i]) error_message (function + " mismatch at offset " + filter_string_convert_to_string (i), desired[i], actual[i]);
  }
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


  // Tests for Database_Config_User. Todo
  {
    // Setup.
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Users database_users = Database_Users ();
    database_users.create ();
    database_users.addNewUser ("username", "password", 5, "");
    request.session_logic ()->attemptLogin ("username", "password");

    // Testing setList, getList, plus add/removeUpdatedSetting.
    vector <string> empty;
    evaluate ("Database_Config_User::getUpdatedSettings 1", empty, request.database_config_user ()->getUpdatedSettings ());

    vector <string> standard1;
    standard1.push_back ("one two three");
    standard1.push_back ("four five six");
    request.database_config_user ()->setUpdatedSettings (standard1);
    evaluate ("Database_Config_User::getUpdatedSettings 2", standard1, request.database_config_user ()->getUpdatedSettings ());

    request.database_config_user ()->addUpdatedSetting ("seven eight nine");
    standard1.push_back ("seven eight nine");
    evaluate ("Database_Config_User::addUpdatedSetting", standard1, request.database_config_user ()->getUpdatedSettings ());
    
    request.database_config_user ()->removeUpdatedSetting ("four five six");
    vector <string> standard2;
    standard2.push_back ("one two three");
    standard2.push_back ("seven eight nine");
    evaluate ("Database_Config_User::removeUpdatedSetting", standard2, request.database_config_user ()->getUpdatedSettings ());
    
    // Testing the Sprint month and its trim () function.
    // It should get today's month.
    int month = filter_string_date_numerical_month ();
    evaluate ("Database_Config_User::getSprintMonth", month, request.database_config_user ()->getSprintMonth ());
    // Set the sprint month to another month value: It should get this value back from the database.
    int newmonth = 123;
    request.database_config_user ()->setSprintMonth (newmonth);
    evaluate ("Database_Config_User::setSprintMonth", newmonth, request.database_config_user ()->getSprintMonth ());
    // Trim: The sprint month should not be reset.
    request.database_config_user ()->trim ();
    evaluate ("Database_Config_User::setSprintMonth", newmonth, request.database_config_user ()->getSprintMonth ());
    // Set the modification time of the sprint month record to more than two days ago: 
    // Trimming resets the sprint month to the current month.
    struct timeval tv;
    gettimeofday (&tv, NULL);
    string filename = filter_url_create_path (testing_directory, "databases", "config", "user", "username", "sprint-month");
    struct stat foo;
    struct utimbuf new_times;
    stat (filename.c_str(), &foo);
    new_times.actime = tv.tv_sec - (2 * 24 * 3600) - 10;
    new_times.modtime = tv.tv_sec - (2 * 24 * 3600) - 10;
    utime (filename.c_str(), &new_times);
    request.database_config_user ()->trim ();
    evaluate ("Database_Config_User::trim", month, request.database_config_user ()->getSprintMonth ());

    // Test boolean setting.
    evaluate ("Database_Config_User::getSubscribeToConsultationNotesEditedByMe", false, request.database_config_user ()->getSubscribeToConsultationNotesEditedByMe ());
    request.database_config_user ()->setSubscribeToConsultationNotesEditedByMe (true);
    evaluate ("Database_Config_User::setSubscribeToConsultationNotesEditedByMe", true, request.database_config_user ()->getSubscribeToConsultationNotesEditedByMe ());
    
    // Test integer setting.
    evaluate ("Database_Config_User::getConsultationNotesPassageSelector", 0, request.database_config_user ()->getConsultationNotesPassageSelector ());
    request.database_config_user ()->setConsultationNotesPassageSelector (11);
    evaluate ("Database_Config_User::setConsultationNotesPassageSelector", 11, request.database_config_user ()->getConsultationNotesPassageSelector ());
    
    // Test string setting.
    evaluate ("Database_Config_User::getConsultationNotesAssignmentSelector", "", request.database_config_user ()->getConsultationNotesAssignmentSelector ());
    request.database_config_user ()->setConsultationNotesAssignmentSelector ("test");
    evaluate ("Database_Config_User::setConsultationNotesAssignmentSelector", "test", request.database_config_user ()->getConsultationNotesAssignmentSelector ());

    evaluate ("Database_Config_User::getSprintYear", filter_string_date_numerical_year (), request.database_config_user ()->getSprintYear ());
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


  // Tests for the filters in the filter folder.
  {
    // Filter_Roles.
    evaluate ("Filter_Roles::consultant", 3, Filter_Roles::consultant ());
    evaluate ("Filter_Roles::lowest", 1, Filter_Roles::lowest ());
  }
  {
    // mkdir including parents.
    string directory = filter_url_create_path (testing_directory, "a", "b");
    filter_url_mkdir (directory);
    string path = filter_url_create_path (directory, "c");
    string contents = "unittest";
    filter_url_file_put_contents (path, contents);
    evaluate ("filter_url_mkdir", contents, filter_url_file_get_contents (path));
  }
  {
    // C++ md5 function as compared to PHP's version.
    evaluate ("md5", "1f3870be274f6c49b3e31a0c6728957f", md5 ("apple"));
  }
  {
    // Test str_replace.
    // Shows that std::string handles UTF-8 well for simple operations. 
    evaluate ("filter_string_str_replace", "⇊⇦", filter_string_str_replace ("⇖", "", "⇊⇖⇦"));
  }
  {
    // Test filter_string_array_unique, a C++ equivalent for PHP's array_unique function.
    vector <string> reference;
    reference.push_back ("aaa");
    reference.push_back ("b");
    reference.push_back ("zzz");
    reference.push_back ("x");
    reference.push_back ("yyy");
    reference.push_back ("k");
    vector <string> input;
    input.push_back ("aaa");
    input.push_back ("b");
    input.push_back ("aaa");
    input.push_back ("b");
    input.push_back ("aaa");
    input.push_back ("zzz");
    input.push_back ("x");
    input.push_back ("x");
    input.push_back ("yyy");
    input.push_back ("k");
    input.push_back ("k");
    vector <string> output = filter_string_array_unique (input);
    evaluate ("filter_string_array_unique", reference, output);
  }
  {
    // Test filter_string_array_diff, a C++ equivalent for PHP's array_diff function.
    vector <string> reference;
    reference.push_back ("aaa");
    reference.push_back ("zzz");
    reference.push_back ("b");
    vector <string> from;
    from.push_back ("aaa");
    from.push_back ("bbb");
    from.push_back ("ccc");
    from.push_back ("zzz");
    from.push_back ("b");
    from.push_back ("x");
    vector <string> against;
    against.push_back ("bbb");
    against.push_back ("ccc");
    against.push_back ("x");
    vector <string> output = filter_string_array_diff (from, against);
    evaluate ("filter_string_array_diff", reference, output);
  }
  {
    // Test trim.
    evaluate ("filter_string_trim 1", "", filter_string_trim ("  "));
    evaluate ("filter_string_trim 2", "", filter_string_trim (""));
    evaluate ("filter_string_trim 2", "xx", filter_string_trim ("\t\nxx\n\r"));
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
  

  // Test for class Session_Logic.
  {
    refresh_sandbox (true);
    
    // The session logic depends on users in the database.
    Database_Users database_users = Database_Users ();
    database_users.create ();
    
    // In an open installation, a client is always logged in as user admin, even after logging out.
    config_globals_open_installation = true;
    Webserver_Request request = Webserver_Request ();
    evaluate ("Session_Logic::loggedIn open", true, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser open", "admin", request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel open", Filter_Roles::admin (), request.session_logic ()->currentLevel ());
    request.session_logic ()->logout ();
    evaluate ("Session_Logic::loggedIn open after logout", true, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser open after logout", "admin", request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel open after logout", Filter_Roles::admin (), request.session_logic ()->currentLevel ());
    
    // Test function to set the username.
    string username = "ঃইঝম";
    request.session_logic ()->setUsername (username);
    evaluate ("Session_Logic::setUsername / currentUser", username, request.session_logic ()->currentUser ());
    config_globals_open_installation = false;
  }
  {
    // In a client installation, a client is logged in as admin when there's no user in the database.
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    config_globals_client_prepared = true;
    Webserver_Request request = Webserver_Request ();
    evaluate ("Session_Logic::loggedIn client", true, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser client", "admin", request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel client", Filter_Roles::admin (), request.session_logic ()->currentLevel ());
    config_globals_client_prepared = false;
  }
  {
    // In a client installation, a client is logged in as the first user in the database.
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    string username = "ঃইঝম";
    int level = 10;
    database_users.addNewUser (username, "password", level, "email");
    config_globals_client_prepared = true;
    Webserver_Request request = Webserver_Request ();
    evaluate ("Session_Logic::loggedIn client user", true, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser client user", username, request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel client user", level, request.session_logic ()->currentLevel ());
    config_globals_client_prepared = false;
  }
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();

    // Enter a user into the database.
    string username = "ঃইঝম";
    string password = "ᨃᨄᨔᨕ";
    string email = "email@website";
    int level = 10;
    database_users.addNewUser (username, password, level, email);

    // Log in by providing username and password.
    evaluate ("Session_Logic::attemptLogin invalid credentials", false, request.session_logic ()->attemptLogin (username, "incorrect"));
    evaluate ("Session_Logic::attemptLogin correct", true, request.session_logic ()->attemptLogin (username, password));

    // Check whether logged in also from another session.
    request = Webserver_Request ();
    evaluate ("Session_Logic::loggedIn normal 2", true, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser normal 2", username, request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel normal 2", level, request.session_logic ()->currentLevel ());
    
    // Logout in another session, and check it in a subsequent session.
    request = Webserver_Request ();
    request.session_logic ()->logout ();
    request = Webserver_Request ();
    evaluate ("Session_Logic::loggedIn normal 3", false, request.session_logic ()->loggedIn ());
    evaluate ("Session_Logic::currentUser normal 3", "", request.session_logic ()->currentUser ());
    evaluate ("Session_Logic::currentLevel normal 3", Filter_Roles::guest(), request.session_logic ()->currentLevel ());
    
    // Login. then vary the browser's signature for subsequent sessions.
    request = Webserver_Request ();
    evaluate ("Session_Logic::attemptLogin 4", true, request.session_logic ()->attemptLogin (username, password));
    evaluate ("Session_Logic::loggedIn 4", true, request.session_logic ()->loggedIn ());
    string remote_address = request.remote_address;
    string user_agent = request.user_agent;
    string accept_language = request.accept_language;
    request = Webserver_Request ();
    request.remote_address = "1.2.3.4";
    evaluate ("Session_Logic::loggedIn 5", false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.remote_address = remote_address;
    evaluate ("Session_Logic::loggedIn 6", true, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.user_agent = "User's Agent";
    evaluate ("Session_Logic::loggedIn 6", false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.user_agent = user_agent;
    evaluate ("Session_Logic::loggedIn 7", true, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.accept_language = "xy_ZA";
    evaluate ("Session_Logic::loggedIn 8", false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.accept_language = accept_language;
    evaluate ("Session_Logic::loggedIn 9", true, request.session_logic ()->loggedIn ());
  }





  // Possible journal entries.
  refresh_sandbox (true);
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;
}


