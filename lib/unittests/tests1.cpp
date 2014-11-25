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
    evaluate (__LINE__, __func__, 3, result.size ());
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
    evaluate (__LINE__, __func__, 1, result.size ());
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
    evaluate (__LINE__, __func__, 1, result.size ());
    lastfilename = "0";
    result = database_logs.get (5, lastfilename);
    evaluate (__LINE__, __func__, 0, result.size ());
    // Rotate that entry away.
    database_logs.rotate ();
    lastfilename = "0";
    result = database_logs.get (6, lastfilename);
    evaluate (__LINE__, __func__, 0, result.size ());

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
    evaluate (__LINE__, __func__, 1, result.size ());
    // Gets it from the filesystem, not the database, because this last entry was not yet rotated.
    lastfilename = "0";
    result = database_logs.get (0, lastfilename);
    evaluate (__LINE__, __func__, 1, result.size ());
    // The "lastsecond" variable, test it.
    int lastsecond = convert_to_int (lastfilename.substr (0, 10));
    if ((lastsecond < now ) || (lastsecond > now + 1)) evaluate (__LINE__, __func__, now, lastsecond);
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
        error_message (__LINE__, __func__, "Should be truncated", s);
      }
    } else {
      evaluate (__LINE__, __func__, 1, result.size ());
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
    if (s == "") evaluate (__LINE__, __func__, "...description", s);
    // Since variable "filename" is updated and set to the last filename,
    // next time function getNext gets nothing.
    s = database_logs.getNext (filename);
    evaluate (__LINE__, __func__, "", s);
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

    evaluate (__LINE__, __func__, true, database_users.matchUsernamePassword (username, password));
    evaluate (__LINE__, __func__, false, database_users.matchUsernamePassword (username, "wrong password"));

    evaluate (__LINE__, __func__, true, database_users.matchEmailPassword (email, password));
    evaluate (__LINE__, __func__, false, database_users.matchEmailPassword (email, "wrong password"));

    string ref = "INSERT INTO users (username, password, level, email) VALUES ('unit test', '014877e71841e82d44ce524d66dcc732', 10, 'email@site.nl');";
    string act = database_users.addNewUserQuery (username, password, level, email);
    evaluate (__LINE__, __func__, ref, act);

    evaluate (__LINE__, __func__, username, database_users.getEmailToUser (email));
    evaluate (__LINE__, __func__, "", database_users.getEmailToUser ("wrong email"));

    evaluate (__LINE__, __func__, email, database_users.getUserToEmail (username));
    evaluate (__LINE__, __func__, "", database_users.getUserToEmail ("wrong username"));

    evaluate (__LINE__, __func__, true, database_users.usernameExists (username));
    evaluate (__LINE__, __func__, false, database_users.usernameExists ("invalid username"));

    evaluate (__LINE__, __func__, true, database_users.emailExists (email));
    evaluate (__LINE__, __func__, false, database_users.emailExists ("invalid email"));

    evaluate (__LINE__, __func__, level, database_users.getUserLevel (username));
    evaluate (__LINE__, __func__, Filter_Roles::guest (), database_users.getUserLevel ("invalid username"));

    level = 7;
    database_users.updateUserLevel (username, level);
    evaluate (__LINE__, __func__, level, database_users.getUserLevel (username));

    database_users.removeUser (username);
    evaluate (__LINE__, __func__, false, database_users.usernameExists (username));
    
    evaluate (__LINE__, __func__, "UPDATE users SET email = 'email@site.nl' WHERE username = 'unit test';", database_users.updateEmailQuery (username, email));
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
    evaluate (__LINE__, __func__, 1, admins.size());
    if (!admins.empty()) evaluate (__LINE__, __func__, username1, admins [0]);
    
    database_users.addNewUser (username2, password, level, email);
    admins = database_users.getAdministrators ();
    evaluate (__LINE__, __func__, 2, admins.size());
    
    email = "new@email.address";
    database_users.updateUserEmail (username1, email);
    evaluate (__LINE__, __func__, email, database_users.getUserToEmail (username1));
    
    vector <string> users = database_users.getUsers ();
    evaluate (__LINE__, __func__, 2, users.size());
    
    evaluate (__LINE__, __func__, md5 (password), database_users.getmd5 (username1));
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
    evaluate (__LINE__, __func__, username, database_users.getUsername (address, agent, fingerprint));
    database_users.removeTokens (username);
    evaluate (__LINE__, __func__, "", database_users.getUsername (address, agent, fingerprint));

    evaluate (__LINE__, __func__, 0, database_users.getTimestamp (username));
    database_users.pingTimestamp (username);
    int timestamp = database_users.getTimestamp (username);
    int second = filter_string_date_seconds_since_epoch ();
    if ((timestamp != second) && (timestamp != second + 1)) evaluate (__LINE__, __func__, second, timestamp);
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
    evaluate (__LINE__, __func__, 0, teams.size());
    
    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 1, teams.size());

    database_users.grantAccess2Bible (username1, bible1);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 1, teams.size());
    if (!teams.empty ()) evaluate (__LINE__, __func__, bible1, teams [0]);
    
    database_users.grantAccess2Bible (username1, bible2);
    teams = database_users.getTeams ();
    evaluate (__LINE__, __func__, 2, teams.size());
    
    vector <string> bibles = database_users.getBibles4User (username1);
    evaluate (__LINE__, __func__, 2, bibles.size());
    if (!bibles.empty ()) evaluate (__LINE__, __func__, bible1, bibles [0]);
    
    database_users.revokeAccess2Bible (username1, bible1);
    bibles = database_users.getBibles4User (username1);
    evaluate (__LINE__, __func__, 1, bibles.size());

    database_users.grantAccess2Bible (username2, bible2);
    vector <string> users = database_users.getUsers4Bible (bible2);
    evaluate (__LINE__, __func__, 2, users.size());
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
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    
    // One team: Any user has access to any Bible.
    database_users.grantAccess2Bible (username1, bible1);
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible2));

    // Two teams: User access control applies.
    database_users.grantAccess2Bible (username1, bible1);
    database_users.grantAccess2Bible (username1, bible2);
    database_users.grantAccess2Bible (username2, bible1);
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible1));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username1, bible2));
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible1));
    evaluate (__LINE__, __func__, false, database_users.hasAccess2Bible (username2, bible2));
    
    // Admin has access to any Bible.
    database_users.addNewUser (username2, "", Filter_Roles::admin (), "");
    evaluate (__LINE__, __func__, true, database_users.hasAccess2Bible (username2, bible2));
    
    // Read only access for known user.
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Bible (username1, bible1));
    database_users.setReadOnlyAccess2Bible (username1, bible1, true);
    evaluate (__LINE__, __func__, true, database_users.hasReadOnlyAccess2Bible (username1, bible1));
    
    // No read-only access for unknown user.
    evaluate (__LINE__, __func__, false, database_users.hasReadOnlyAccess2Bible ("unknown", bible1));
  }
}


void test_sqlite ()
{
  // Tests for SQLite.
  sqlite3 * db = database_sqlite_connect ("sqlite");
  if (!db) error_message (__LINE__, __func__, "pointer", "NULL");
  database_sqlite_exec (db, "CREATE TABLE test (column1 integer, column2 integer, column3 integer);");
  database_sqlite_exec (db, "INSERT INTO test VALUES (123, 456, 789);");
  database_sqlite_exec (db, "INSERT INTO test VALUES (234, 567, 890);");
  database_sqlite_exec (db, "INSERT INTO test VALUES (345, 678, 901);");
  map <string, vector <string> > actual = database_sqlite_query (db, "SELECT column1, column2, column3 FROM test;");
  evaluate (__LINE__, __func__, "567", actual ["column2"] [1]);
  database_sqlite_disconnect (db);
  database_sqlite_disconnect (NULL);

  evaluate (__LINE__, __func__, true, database_sqlite_healthy ("sqlite"));
  unlink (database_sqlite_file ("sqlite").c_str());
  evaluate (__LINE__, __func__, false, database_sqlite_healthy ("sqlite"));

  evaluate (__LINE__, __func__, "He''s", database_sqlite_no_sql_injection ("He's"));
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
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, "admin", request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, Filter_Roles::admin (), request.session_logic ()->currentLevel ());
    request.session_logic ()->logout ();
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, "admin", request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, Filter_Roles::admin (), request.session_logic ()->currentLevel ());
    
    // Test function to set the username.
    string username = "ঃইঝম";
    request.session_logic ()->setUsername (username);
    evaluate (__LINE__, __func__, username, request.session_logic ()->currentUser ());
    config_globals_open_installation = false;
  }
  {
    // In a client installation, a client is logged in as admin when there's no user in the database.
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    config_globals_client_prepared = true;
    Webserver_Request request = Webserver_Request ();
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, "admin", request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, Filter_Roles::admin (), request.session_logic ()->currentLevel ());
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
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, username, request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, level, request.session_logic ()->currentLevel ());
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
    evaluate (__LINE__, __func__, false, request.session_logic ()->attemptLogin (username, "incorrect"));
    evaluate (__LINE__, __func__, true, request.session_logic ()->attemptLogin (username, password));

    // Check whether logged in also from another session.
    request = Webserver_Request ();
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, username, request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, level, request.session_logic ()->currentLevel ());
    
    // Logout in another session, and check it in a subsequent session.
    request = Webserver_Request ();
    request.session_logic ()->logout ();
    request = Webserver_Request ();
    evaluate (__LINE__, __func__, false, request.session_logic ()->loggedIn ());
    evaluate (__LINE__, __func__, "", request.session_logic ()->currentUser ());
    evaluate (__LINE__, __func__, Filter_Roles::guest(), request.session_logic ()->currentLevel ());
    
    // Login. then vary the browser's signature for subsequent sessions.
    request = Webserver_Request ();
    evaluate (__LINE__, __func__, true, request.session_logic ()->attemptLogin (username, password));
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    string remote_address = request.remote_address;
    string user_agent = request.user_agent;
    string accept_language = request.accept_language;
    request = Webserver_Request ();
    request.remote_address = "1.2.3.4";
    evaluate (__LINE__, __func__, false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.remote_address = remote_address;
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.user_agent = "User's Agent";
    evaluate (__LINE__, __func__, false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.user_agent = user_agent;
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.accept_language = "xy_ZA";
    evaluate (__LINE__, __func__, false, request.session_logic ()->loggedIn ());
    request = Webserver_Request ();
    request.accept_language = accept_language;
    evaluate (__LINE__, __func__, true, request.session_logic ()->loggedIn ());
  }
}


void test_empty_folders ()
{
  // There should be no empty folders in the library, because git does not include them.
  int result = system ("find . -type d -empty");
  evaluate (__LINE__, __func__, 0, result);
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
  evaluate (__LINE__, __func__, desired, actual);
  
  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("two");
  desired = "line 1\n\nline 2\n\n\n\nline 3\n\nline 4\n\n\nline 6";
  actual = filter_string_trim (flate.render (tpl));
  evaluate (__LINE__, __func__, desired, actual);

  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("three");
  flate.set_variable ("three", "THREE");
  desired = "line 1\n\nline 2\n\n\n\nline 4\n\nTHREE\nline 5\n\n\nline 6";
  actual = filter_string_trim (flate.render (tpl));
  evaluate (__LINE__, __func__, desired, actual);
}


