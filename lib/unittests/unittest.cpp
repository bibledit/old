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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <unittests/libraries.h>
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
#include <config.h>
#include <libxml/parser.h>


#ifdef HAVE_UNITTESTS


// Tests for Database_Bibles.
void test_database_bibles ()
{
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    vector <string> standard;
    vector <string> bibles = database_bibles.getBibles ();
    evaluate (__LINE__, __func__, standard, bibles);
  }
  {
    // Test whether optimizing works without errors.
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
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
    Database_Search database_search = Database_Search ();
    database_search.create ();
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
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
    
    database_bibles.setID ("phpunit", 10);
    
    id = database_bibles.getID ("phpunit");
    evaluate (__LINE__, __func__, 10, id);
  }
  // Test storeChapter / getChapter
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
    Database_Search database_search = Database_Search ();
    database_search.create ();

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
}


#endif


int main (int argc, char **argv) 
{
  // No compile warnings.
  if (argc) {};
  if (argv[0]) {};

#ifdef HAVE_UNITTESTS

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
  
  // test_editor_html2usfm ();
  test_editor_usfm2html ();
  // test_editor_roundtrip ();
  // test_editor_roundtrip_verse ();
  refresh_sandbox (true); exit (0); // Todo
  
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
  test_flate ();
  test_database_styles ();
  test_database_bibles ();
  test_database_search ();
  test_database_books ();
  test_database_bibleactions ();
  test_database_check ();
  test_database_localization ();
  test_database_confirm ();
  test_database_ipc ();
  test_database_jobs ();
  test_database_kjv ();
  test_database_morphhb ();
  test_database_sblgnt ();
  test_database_offlineresourcese ();
  test_database_sprint ();
  test_database_mail ();
  test_database_navigation ();
  test_database_resources ();
  test_database_usfmresources ();
  test_database_mappings ();
  test_database_noteactions ();
  test_database_versifications ();
  test_database_modifications_user ();
  test_database_modifications_team ();
  test_database_modifications_notifications ();
  test_database_notes ();
  test_database_volatile ();
  test_database_state ();
  test_database_imageresources ();
  test_styles_css ();
  test_filter_custom_css ();
  test_filter_diff ();
  test_filter_abbreviations ();
  test_checksum_logic ();
  test_store_bible_data ();
  test_editor_html2usfm ();
  test_editor_usfm2html ();
  test_editor_roundtrip ();
  test_editor_roundtrip_verse ();
  test_filter_markup ();
  test_workbench_logic ();
  test_filter_git ();
  test_filter_merge ();
  test_filter_tidy ();
  test_ipc_notes ();
  test_check_verses ();
  test_filter_date ();
  test_client_logic ();
  test_check_sentences ();
  test_check_versification ();
  test_check_usfm ();
  test_filter_url  ();
  test_hyphenate ();
  test_filter_string ();
  test_database_noteassignment ();
  test_database_strong ();
  test_database_morphgnt ();
  test_database_etcbc4 ();
  test_database_hebrewlexicon ();
  test_filter_string_text2html ();
  test_database_cache ();
  test_filter_archive ();

  
  // Output possible journal entries.
  refresh_sandbox (true);
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;

#else
  
  cout << "Unit tests are disabled" << endl;
  cout << "Enable them through ./configure --enable-unittests" << endl;
  return 0;

#endif
}


