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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <unittests/tests1.h>
#include <unittests/database_config.h>
#include <unittests/filters.h>
#include <unittests/databases.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/search.h>
#include <database/books.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <session/logic.h>



// Tests for Database_Bibles.
void test_database_bibles ()
{
  {
    refresh_sandbox (true);
    /* C++Port repeat for every test.
    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibleactions = Database_BibleActions::getInstance ();
    $database_bibleactions->create ();
    */
    Database_Bibles database_bibles = Database_Bibles ();
    vector <string> standard;
    vector <string> bibles = database_bibles.getBibles ();
    evaluate (__LINE__, __func__, standard, bibles);
  }
  {
    // Test whether optimizing works without errors.
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    int id = database_bibles.createBible ("phpunit");
    if (id == 0) evaluate (__LINE__, __func__, "non-zero", id);
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "g");
    database_bibles.optimize ();
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "g", usfm);
  }
  {
    // Test whether optimizing removes files with 0 size.
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    int id = database_bibles.createBible ("phpunit");
    if (id == 0) evaluate (__LINE__, __func__, "non-zero", id);
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "");
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "", usfm);
    database_bibles.optimize ();
    usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "f", usfm);
  }
  // Test create / get / delete Bibles.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    int id = database_bibles.createBible ("phpunit");
    evaluate (__LINE__, __func__, 1, id);
    
    vector <string> bibles = database_bibles.getBibles ();
    vector <string> standard = {"phpunit"};
    evaluate (__LINE__, __func__, standard, bibles);

    id = database_bibles.getID ("phpunit2");
    evaluate (__LINE__, __func__, 0, id);
    
    database_bibles.deleteBible ("phpunit");

    id = database_bibles.getID ("phpunit");
    evaluate (__LINE__, __func__, 0, id);
  }
  // Test names / identifiers.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    int id = database_bibles.getID ("phpunit");
    evaluate (__LINE__, __func__, 0, id);

    string bible = database_bibles.getName (0);
    evaluate (__LINE__, __func__, "Unknown", bible);

    id = database_bibles.createBible ("phpunit");
    evaluate (__LINE__, __func__, 1, id);

    id = database_bibles.getID ("phpunit");
    evaluate (__LINE__, __func__, 1, id);

    bible = database_bibles.getName (1);
    evaluate (__LINE__, __func__, "phpunit", bible);
    
    bible = database_bibles.getName (2);
    evaluate (__LINE__, __func__, "Unknown", bible);
  }
  // Test storeChapter / getChapter
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    string usfm = "\\c 1\n\\p\n\\v 1 Verse 1";
    database_bibles.storeChapter ("phpunit", 2, 1, usfm);
    string result = database_bibles.getChapter ("phpunit", 2, 1);
    evaluate (__LINE__, __func__, usfm, result);
    result = database_bibles.getChapter ("phpunit2", 2, 1);
    evaluate (__LINE__, __func__, "", result);
    result = database_bibles.getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, "", result);
  }
  // Test books
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    vector <int> books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { }, books);

    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1 }, books);

    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 0");
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1, 2 }, books);

    database_bibles.deleteBook ("phpunit", 3);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1, 2 }, books);

    database_bibles.deleteBook ("phpunit", 1);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);

    database_bibles.deleteBook ("phpunit2", 2);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);
  }
  // Test chapters ()
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    database_bibles.createBible ("phpunit");
    vector <int> chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { }, chapters);
 
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2 }, chapters);
    
    chapters = database_bibles.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { }, chapters);

    database_bibles.storeChapter ("phpunit", 1, 3, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2, 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 3, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2, 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 1, 2);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 1, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { }, chapters);
  }
  // Test chapter IDs
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    database_bibles.createBible ("phpunit");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    int id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000001, id);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000002, id);

    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000004, id);

    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000004, id);
  }
  // Test Bible actionsOne
  {
    /* C++Port
    database_bibleactions = Database_BibleActions::getInstance ();

    database_bibleactions.optimize ();
    
    bibles = database_bibleactions.getBibles ();
    this.assertEquals (array (), bibles);

    database_bibleactions.record ("phpunit1", 1, 2, "data112");
    database_bibleactions.record ("phpunit1", 1, 3, "data113");
    database_bibleactions.record ("phpunit1", 2, 4, "data124");
    database_bibleactions.record ("phpunit2", 5, 6, "data256");
    database_bibleactions.record ("phpunit2", 5, 6, "data256: Not to be stored");

    bibles = database_bibleactions.getBibles ();
    this.assertEquals (array ("phpunit1", "phpunit2"), bibles);

    books = database_bibleactions.getBooks ("phpunit1");
    this.assertEquals (array (1, 2), books);

    chapters = database_bibleactions.getChapters ("phpunit1", 1);
    this.assertEquals (array (2, 3), chapters);
    
    chapters = database_bibleactions.getChapters ("phpunit1", 2);
    this.assertEquals (array (4), chapters);
    
    database_bibleactions.delete ("phpunit1", 2, 3);
    
    chapters = database_bibleactions.getChapters ("phpunit1", 2);
    this.assertEquals (array (4), chapters);
    
    database_bibleactions.delete ("phpunit1", 2, 4);
    
    chapters = database_bibleactions.getChapters ("phpunit1", 2);
    this.assertEquals (array (), chapters);
    
    usfm = database_bibleactions.getUsfm ("phpunit2", 5, 5);
    this.assertFalse (usfm);
    
    usfm = database_bibleactions.getUsfm ("phpunit2", 5, 6);
    this.assertEquals ("data256", usfm);
    */
  }
}


int main (int argc, char **argv) 
{
  // No compile warnings.
  if (argc) {};
  if (argv[0]) {};

  cout << "Running unittests" << endl;

  // Directory where the unit tests will run.
  testing_directory = "/tmp/bibledit-unittests";  
  filter_url_mkdir (testing_directory);
  refresh_sandbox (true);
  config_globals_document_root = testing_directory;

  // Number of failed unit tests.  
  error_count = 0;
  
  // Flag for unit tests.
  config_globals_unit_testing = true;

  test_database_search (); exit (0); // Todo

  // Run the tests.
  test_database_config_general ();
  test_database_config_bible ();
  test_database_config_user ();
  test_sqlite ();
  test_database_logs ();
  test_filters ();
  test_database_users ();
  test_session_logic ();
  test_empty_folders ();
  test_flate2 ();
  test_database_styles ();
  test_database_bibles ();
  test_database_search ();
  test_database_books ();

  // Output possible journal entries.
  refresh_sandbox (true);
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;
}


