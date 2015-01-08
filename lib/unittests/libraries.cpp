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
#include <filter/usfm.h>
#include <flate/flate.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <checksum/logic.h>
#include <editor/export.h>
#include <editor/import.h>


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


void test_checksum_logic ()
{
  // Get1
  {
    string data = "\\v Verse 1";
    string checksum = Checksum_Logic::get (data);
    evaluate (__LINE__, __func__, "3", checksum);
  }
  // Get2
  {
    string data = "Line one\\nLine 2\\n";
    string checksum = Checksum_Logic::get (data);
    evaluate (__LINE__, __func__, "3", checksum);
  }
  // Send1
  {
    string data = "\\v Verse 1";
    string checksum = Checksum_Logic::send (data);
    string standard = "3\n" + data;
    evaluate (__LINE__, __func__, standard, checksum);
  }
  // Send2
  {
    string data = "Line one\\nLine 2\n";
    string checksum = Checksum_Logic::send (data);
    string standard = "3\n" + data;
    evaluate (__LINE__, __func__, standard, checksum);
  }
  // Setup some data.
  refresh_sandbox (true);
  Webserver_Request request;
  request.database_search()->create ();
  request.database_bibles()->storeChapter ("phpunit1", 1, 2, "data1");
  request.database_bibles()->storeChapter ("phpunit1", 1, 3, "data2");
  request.database_bibles()->storeChapter ("phpunit1", 1, 4, "data3");
  request.database_bibles()->storeChapter ("phpunit2", 2, 5, "data4");
  // GetChapter1
  {
    string checksum = Checksum_Logic::getChapter (&request, "phpunit1", 1, 2);
    evaluate (__LINE__, __func__, md5 ("data1"), checksum);
  }
  // GetChapter2
  {
    string checksum = Checksum_Logic::getChapter (&request, "phpunit2", 2, 6);
    evaluate (__LINE__, __func__, md5 (""), checksum);
  }
  // GetBook1
  {
    string checksum = Checksum_Logic::getBook (&request, "phpunit1", 1);
    evaluate (__LINE__, __func__, "2ab6425924e6cd38b2474c543c5ea602", checksum);
  }
  // GetBook2
  {
    string checksum = Checksum_Logic::getBook (&request, "phpunit3", 1);
    evaluate (__LINE__, __func__, "d41d8cd98f00b204e9800998ecf8427e", checksum);
  }
  // GetBible1
  {
    string checksum = Checksum_Logic::getBible (&request, "phpunit1");
    evaluate (__LINE__, __func__, "f9dc679a8712eb6f65b584e9688e9680", checksum);
  }
  // GetBible2
  {
    string checksum = Checksum_Logic::getBible (&request, "phpunit2");
    evaluate (__LINE__, __func__, "ee84a85bac14adb35e887c3d89bc80ab", checksum);
  }
  // GetBibles1
  {
    string checksum = Checksum_Logic::getBibles (&request, {"phpunit1", "phpunit2"});
    evaluate (__LINE__, __func__, "440b2008993816f0bc8c557b64fbdaf2", checksum);
  }
  // GetBibles2
  {
    string checksum = Checksum_Logic::getBibles (&request, {"phpunit3", "phpunit4"});
    evaluate (__LINE__, __func__, "020eb29b524d7ba672d9d48bc72db455", checksum);
  }
}


void test_store_bible_data_safely_setup (Webserver_Request * request, string usfm)
{
  refresh_sandbox (true);
  request->database_search()->create ();
  request->database_bibles()->createBible ("phpunit");
  request->database_bibles()->storeChapter ("phpunit", 1, 1, usfm);
}


void test_store_bible_data ()
{
  Webserver_Request request;
  string usfm =
  "\\c 1\n"
  "\\p\n"
  "\\v 1 Verse 1.\n"
  "\\v 2 Verse 2.\n"
  "\\v 3 Verse 3.\n"
  "\\v 4 Verse 4.\n"
  "\\v 5 Verse 5.\n";
  // SafeStoreChapterSaveOne
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\v 3 Verse 3.\n"
    "\\v 4 Verse 4.\n"
    "\\v 5 Verse 5.\n";
    bool stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data);
    evaluate (__LINE__, __func__, true, stored);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, data, result);
  }
  // SafeStoreChapterSaveTwo
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\v 3 Verse 3.\n"
    "\\v 4 Verse 4.\n";
    bool stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data);
    evaluate (__LINE__, __func__, true, stored);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, data, result);
  }
  // SafeStoreChapterLength
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\v 3 Verse 3.\n";
    bool stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data);
    evaluate (__LINE__, __func__, false, stored);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    refresh_sandbox (false);
  }
  // SafeStoreChapterSimilarity
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 3 Verse 3.\n"
    "\\v 2 Verse 2.\n"
    "\\v 4 Verse 4.\n";
    bool stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data);
    evaluate (__LINE__, __func__, false, stored);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    refresh_sandbox (false);
  }
  // SafeStoreChapterNoChange
  {
    test_store_bible_data_safely_setup (&request, usfm);
    int currentId = request.database_bibles()->getChapterId ("phpunit", 1, 1);
    bool stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, usfm);
    evaluate (__LINE__, __func__, true, stored);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    int currentId2 = request.database_bibles()->getChapterId ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, currentId, currentId2);
  }
}


void test_editor_export_import () // Todo
{
  /* Todo temporarily switched off.
  // Basic test.
  {
    Webserver_Request request;
    string html = "<p class=\"p\"><span>The earth brought forth.</span></p>";
    Editor_Export editor_export (&request);
    editor_export.load (html);
    editor_export.stylesheet ("Standard");
    editor_export.run ();
    string usfm = editor_export.get ();
    string standard = "\\p The earth brought forth.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  // Non-Breaking Spaces
  {
    Webserver_Request request;
    string html = "<p class=\"p\"><span>The&nbsp;earth &nbsp; brought&nbsp;&nbsp;forth.</span></p>";
    Editor_Export editor_export (&request);
    editor_export.load (html);
    editor_export.stylesheet ("Standard");
    editor_export.run ();
    string usfm = editor_export.get ();
    string standard = "\\p The earth brought forth.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  */
  // Embedded Text Spans One
  {
    Webserver_Request request;
    string html = "<p class=\"p\"><span>The <span class=\"add\"><span class=\"nd\">Lord God</span> is calling</span> you</span><span>.</span></p>";
    Editor_Export editor_export (&request);
    editor_export.load (html);
    editor_export.stylesheet ("Standard");
    editor_export.run ();
    string usfm = editor_export.get ();
    string standard = "\\p The \\add \\+nd Lord God\\+nd* is calling\\add* you.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
}
/* Todo

 Editor export tests.
 
 public function testFootnoteDeletedBody ()
 {
 $html = <<<'EOD'
 <p class="p"><span>The earth brought forth</span><a href="#note1" id="citation1" class="superscript">f</a><span>.</span></p>
 <div id="notes">
 <hr>
 <p class="f"></p>
 <br>
 </div>
 EOD;
 $editor_export = Editor_Export::getInstance ();
 $editor_export->load ($html);
 $editor_export->stylesheet ("Standard");
 $editor_export->run ();
 $usfm = $editor_export->get ();
 $standard = <<<'EOD'
 \p The earth brought forth.
 \f +\f*
 EOD;
 $this->assertEquals ($standard, filter_string_trim ($usfm));
 }
 
 
 public function testFootnoteDeletedCitation ()
 {
 $html = <<<'EOD'
 <p class="p"><span>The earth brought forth</span><span>.</span></p>
 <div id="notes">
 <hr>
 <p class="f"><a href="#citation1" id="note1">f</a><span> </span><span>+ </span><span class="fk">brought: </span><span class="fl">Heb. </span><span class="fq">explanation.</span></p>
 </div>
 EOD;
 $editor_export = Editor_Export::getInstance ();
 $editor_export->load ($html);
 $editor_export->stylesheet ("Standard");
 $editor_export->run ();
 $usfm = $editor_export->get ();
 $standard = <<<'EOD'
 \p The earth brought forth.
 \f + \fk brought: \fl Heb. \fq explanation.\f*
 EOD;
 $this->assertEquals ($standard, filter_string_trim ($usfm));
 }
 
 
 

 */