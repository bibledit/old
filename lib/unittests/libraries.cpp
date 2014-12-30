/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <unittests/libraries.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <flate/flate.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>


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
  int result = system ("find . -type d -empty > /tmp/bibledittest.txt");
  evaluate (__LINE__, __func__, 0, result);
  string contents = filter_url_file_get_contents ("/tmp/bibledittest.txt");
  evaluate (__LINE__, __func__, "", contents);
  filter_url_unlink ("/tmp/bibledittest.txt");
}


void test_flate ()
{
  // Test for the flate2 template engine.
  string tpl = filter_url_create_root_path ("unittests", "tests", "flate1.html");
  Flate flate;
  string desired;
  string actual;

  flate = Flate ();
  desired = 
    "line 1\n"
    "\n"
    "line 6\n"
    "LocalizeOne\n"
    "LocalizeTwo";
  actual = filter_string_trim (flate.render (tpl));
  evaluate (__LINE__, __func__, desired, actual);

  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("two");
  desired = 
    "line 1\n"
    "\n"
    "line 2\n"
    "\n"
    "\n"
    "\n"
    "line 3\n"
    "\n"
    "line 4\n"
    "\n"
    "\n"
    "line 6\n"
    "LocalizeOne\n"
    "LocalizeTwo";
  actual = filter_string_trim (flate.render (tpl));
  evaluate (__LINE__, __func__, desired, actual);

  flate = Flate ();
  flate.enable_zone ("one");
  flate.enable_zone ("three");
  flate.set_variable ("three", "THREE");
  desired = 
    "line 1\n"
    "\n"
    "line 2\n"
    "\n"
    "\n"
    "\n"
    "line 4\n"
    "\n"
    "THREE\n"
    "line 5\n"
    "\n"
    "\n"
    "line 6\n"
    "LocalizeOne\n"
    "LocalizeTwo";
  actual = filter_string_trim (flate.render (tpl));
  evaluate (__LINE__, __func__, desired, actual);
}


