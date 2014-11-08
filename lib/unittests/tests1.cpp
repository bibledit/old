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


#include <unittests/tests1.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <flate/flate2.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>


void test_database_config_general ()
{
  // Tests for Database_Config_General.
  evaluate ("Database_Config_General::getSiteMailName", "Bible Translation", Database_Config_General::getSiteMailName ());

  string ref = "unittest";
  Database_Config_General::setSiteMailName (ref);
  evaluate ("Database_Config_General::getSiteMailName", ref, Database_Config_General::getSiteMailName ());

  evaluate ("Database_Config_General::getMailStorageSecurity", "", Database_Config_General::getMailStorageSecurity ());
}


void test_database_config_bible ()
{
  // Tests for Database_Config_Bible.
    evaluate ("Database_Config_Bible::getViewableByAllUsers", "", Database_Config_Bible::getViewableByAllUsers ("testbible"));

    string ref = "1";
    Database_Config_Bible::setViewableByAllUsers ("testbible", ref);
    evaluate ("Database_Config_Bible::getViewableByAllUsers", ref, Database_Config_Bible::getViewableByAllUsers ("testbible"));
}


void test_database_config_user ()
{
  // Tests for Database_Config_User.
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
    string filename = filter_url_create_path (testing_directory, "databases", "config", "user", "username", "sprint-month");
    struct stat foo;
    struct utimbuf new_times;
    stat (filename.c_str(), &foo);
    new_times.actime = filter_string_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
    new_times.modtime = filter_string_date_seconds_since_epoch () - (2 * 24 * 3600) - 10;
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
    
    // Test getting a Bible that does not exist: It creates one.
    evaluate ("Database_Config_User::getBible", "testBible", request.database_config_user ()->getBible ());
  }
}


void test_database_logs ()
{
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
    string lastfilename;
    vector <string> result = database_logs.get (0, lastfilename);
    evaluate ("Database_Logs::get", 3, result.size ());
    refresh_sandbox (false);
  }
  {
    // Check the database: It should recreate the database and then create one entry in the Journal.
    // This entry is proof that it recreated the database.
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.checkup ();
    string lastfilename = "1111111111";
    vector <string> result = database_logs.get (0, lastfilename);
    evaluate ("Database_Logs::get", 1, result.size ());
    refresh_sandbox (false);
  }
  {
    refresh_sandbox (true);
    Database_Logs database_logs = Database_Logs ();
    database_logs.create ();
    int now = filter_string_date_seconds_since_epoch ();
    int min1days = now - 86400 - 10;
    int min2days = min1days - 86400;
    int min3days = min2days - 86400;
    int min4days = min3days - 86400;
    int min5days = min4days - 86400;
    int min6days = min5days - 86400;
    string lastfilename;
    vector <string> result;

    // Log entry for 6 days ago.
    Database_Logs::log ("Six days ago");
    database_logs.rotate ();
    database_logs.update (now, min6days);
    lastfilename = "0";
    result = database_logs.get (6, lastfilename);
    evaluate ("Database_Logs::get: Six days ago: Size should be 1", 1, result.size ());
    lastfilename = "0";
    result = database_logs.get (5, lastfilename);
    evaluate ("Database_Logs::get: Five days ago: Size should be 0", 0, result.size ());
    // Rotate that entry away.
    database_logs.rotate ();
    lastfilename = "0";
    result = database_logs.get (6, lastfilename);
    evaluate ("Database_Logs::get: Rotate six days away: Size should be 0", 0, result.size ());

    // Log entry for 2 days, 1 day ago, and now.
    Database_Logs::log ("Two days ago");
    database_logs.rotate ();
    database_logs.update (now, min2days);
    Database_Logs::log ("One day ago");
    database_logs.rotate ();
    database_logs.update (now, min1days);
    Database_Logs::log ("Now");
    lastfilename = "0";
    result = database_logs.get (2, lastfilename);
    evaluate ("Database_Logs::get: Two days ago: Size should be 1", 1, result.size ());
    // Gets it from the filesystem, not the database, because this last entry was not yet rotated.
    lastfilename = "0";
    result = database_logs.get (0, lastfilename);
    evaluate ("Database_Logs::get: Now: Size should be 1", 1, result.size ());
    // The "lastsecond" variable, test it.
    int lastsecond = convert_to_int (lastfilename.substr (0, 10));
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
    string s = "0";
    vector <string> result = database_logs.get (0, s);
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
    int second = filter_string_date_seconds_since_epoch ();
    string filename = convert_to_string (second) + "00000000";
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
}


void test_database_users ()
{
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
    database_users.pingTimestamp (username);
    int timestamp = database_users.getTimestamp (username);
    int second = filter_string_date_seconds_since_epoch ();
    if ((timestamp != second) && (timestamp != second + 1)) evaluate ("Database_Users::getTimestamp time", second, timestamp);
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
}


void test_sqlite ()
{
  // Tests for SQLite.
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


void test_session_logic ()
{
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
}


void test_empty_folders ()
{
  // There should be no empty folders in the library, because git does not include them.
  int result = system ("find . -type d -empty");
  evaluate ("No empty folders", 0, result);
}


void test_flate2 ()
{
  // Test for the flate2 template engine.
  string tpl = filter_url_create_root_path ("unittests", "tests", "flate1.html");
  Flate flate = Flate ();
  string desired;
  string actual;

  desired = "line 1\n\nline 6";
  actual = filter_string_trim (flate.render (tpl));
  evaluate ("Flate 1", desired, actual);
  
  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("two");
  desired = "line 1\n\nline 2\n\n\n\nline 3\n\nline 4\n\n\nline 6";
  actual = filter_string_trim (flate.render (tpl));
  evaluate ("Flate 2", desired, actual);

  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("three");
  flate.set_variable ("three", "THREE");
  desired = "line 1\n\nline 2\n\n\n\nline 4\n\nTHREE\nline 5\n\n\nline 6";
  actual = filter_string_trim (flate.render (tpl));
  evaluate ("Flate 3", desired, actual);
}


void test_database_styles ()
{
  // Tests for Database_Styles.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.optimize ();
    database_styles.createStandardSheet ();
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();

    vector <string> sheets = database_styles.getSheets ();
    vector <string> standard;
    standard.push_back ("Standard");
    evaluate ("Database_Styles::getSheets 1", standard, sheets);

    database_styles.createSheet ("phpunit");
    standard.push_back ("phpunit");
    sheets = database_styles.getSheets ();
    evaluate ("Database_Styles::getSheets 2", standard, sheets);

    database_styles.deleteSheet ("phpunit");
    standard.pop_back ();
    sheets = database_styles.getSheets ();
    evaluate ("Database_Styles::getSheets 3", standard, sheets);

    database_styles.deleteSheet ("Standard");
    sheets = database_styles.getSheets ();
    evaluate ("Database_Styles::getSheets 4", standard, sheets);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    vector <string> markers = database_styles.getMarkers ("phpunit");
    evaluate ("Database_Styles::getMarkers 5", 179, markers.size ());
    string marker = "p";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate ("Database_Styles::getMarkers 6", marker, "not found");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate ("Database_Styles::getMarkers 7", marker, "not found");

    map <string, vector <string> > markers_names = database_styles.getMarkersAndNames ("phpunit");
    evaluate ("Database_Styles::getMarkersAndNames 1", markers, markers_names ["marker"]);

    marker = "p";
    vector <string>::iterator iter = find (markers_names ["marker"].begin (), markers_names ["marker"].end (), marker);
    size_t index = distance (markers_names ["marker"].begin (), iter);
    string name = markers_names ["name"] [index];
    evaluate ("Database_Styles::getMarkersAndNames 2", "Normal, First Line Indent", name);

    marker = "add";
    iter = find (markers_names ["marker"].begin (), markers_names ["marker"].end (), marker);
    index = distance (markers_names ["marker"].begin (), iter);
    name = markers_names ["name"] [index];
    evaluate ("Database_Styles::getMarkersAndNames 3", "* Translational Addition", name);
    
    database_styles.deleteMarker ("phpunit", "p");
    
    markers = database_styles.getMarkers ("phpunit");
    marker = "p";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate ("Database_Styles::getMarkers 8", marker, "should not be there");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate ("Database_Styles::getMarkers 9", marker, "not found");
    
    markers_names = database_styles.getMarkersAndNames ("phpunit");

    marker = "p";
    iter = find (markers_names ["marker"].begin (), markers_names ["marker"].end (), marker);
    if (iter != markers_names ["marker"].end ()) evaluate ("Database_Styles::getMarkersAndNames 4", marker, "should not be there");

    marker = "add";
    iter = find (markers_names ["marker"].begin (), markers_names ["marker"].end (), marker);
    index = distance (markers_names ["marker"].begin (), iter);
    name = markers_names ["name"] [index];
    evaluate ("Database_Styles::getMarkersAndNames 5", "* Translational Addition", name);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");
    
    vector <string> markers = database_styles.getMarkers ("phpunit");
    for (string marker : markers) if (marker != "add") database_styles.deleteMarker ("phpunit", marker);
    string xml = database_styles.exportXml ("phpunit");
    string standard = filter_url_file_get_contents (filter_url_create_root_path ("unittests", "tests", "addstyle.xml"));
    evaluate ("Database_Styles::exportXml", filter_string_trim (standard), filter_string_trim (xml));
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate ("Database_Styles::getMarkerData 1", "add", data.marker);
    evaluate ("Database_Styles::getMarkerData 2", "st", data.category);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");
    database_styles.updateName ("phpunit", "add", "Addition");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate ("Database_Styles::updateName", "Addition", data.name);

    database_styles.updateInfo ("phpunit", "p", "Paragraph");
    data = database_styles.getMarkerData ("phpunit", "p");
    evaluate ("Database_Styles::updateInfo", "Paragraph", data.info);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // A user does not have write access to the stylesheet.
    bool write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate ("Database_Styles::hasWriteAccess", false, write);

    // Grant write access, and test it for this user, and for another user.
    database_styles.grantWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate ("Database_Styles::grantWriteAccess", true, write);
    write = database_styles.hasWriteAccess ("user2", "phpunit");
    evaluate ("Database_Styles::grantWriteAccess 2", false, write);
    write = database_styles.hasWriteAccess ("user", "phpunit2");
    evaluate ("Database_Styles::hasWriteAccess 2", false, write);

    // Revoke write access for a user, test it in various ways.
    database_styles.revokeWriteAccess ("user2", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate ("Database_Styles::revokeWriteAccess", true, write);
    database_styles.revokeWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate ("Database_Styles::revokeWriteAccess 2", false, write);
    
    // Revoking write access for all users.
    database_styles.grantWriteAccess ("user1", "phpunit");
    database_styles.grantWriteAccess ("user2", "phpunit");
    database_styles.revokeWriteAccess ("", "phpunit");
    write = database_styles.hasWriteAccess ("user1", "phpunit");
    evaluate ("Database_Styles::revokeWriteAccess all", false, write);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // Get markers.
    vector <string> markers = database_styles.getMarkers ("phpunit");
    string marker = "zhq";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate ("Database_Styles::addMarker 1", marker, "should not be there");
  
    // Add marker.
    database_styles.addMarker ("phpunit", marker);
    markers = database_styles.getMarkers ("phpunit");
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate ("Database_Styles::addMarker 1", marker, "should be there");
  }
}


void test_database_books ()
{
  refresh_sandbox (true);
  Database_Books database_books = Database_Books ();
  database_books.create ();
  evaluate ("Database_Books::getIDs", 69, database_books.getIDs ().size());
  evaluate ("Database_Books::getIdFromEnglish", 2, database_books.getIdFromEnglish ("Exodus"));
  evaluate ("Database_Books::getEnglishFromId", "Leviticus", database_books.getEnglishFromId (3));
  evaluate ("Database_Books::getUsfmFromId", "NUM", database_books.getUsfmFromId (4));
  evaluate ("Database_Books::getBibleworksFromId", "Deu", database_books.getBibleworksFromId (5));
  evaluate ("Database_Books::getIdFromUsfm", 22, database_books.getIdFromUsfm ("SNG"));
  evaluate ("Database_Books::getIdFromOsis", 13, database_books.getIdFromOsis ("1Chr"));
  evaluate ("Database_Books::getIdFromBibleworks", 12, database_books.getIdFromBibleworks ("2Ki"));
  evaluate ("Database_Books::getIdLikeText", 12, database_books.getIdLikeText ("2Ki"));
  evaluate ("Database_Books::getIdFromOnlinebible", 12, database_books.getIdFromOnlinebible ("2Ki"));
  evaluate ("Database_Books::getOnlinebibleFromId", "De", database_books.getOnlinebibleFromId (5));
  evaluate ("Database_Books::getSequenceFromId", "5", database_books.getSequenceFromId (5));
  evaluate ("Database_Books::getType 1", "nt", database_books.getType (40));
  evaluate ("Database_Books::getType 1", "ot", database_books.getType (39));
  evaluate ("Database_Books::getType 1", "", database_books.getType (0));
}


