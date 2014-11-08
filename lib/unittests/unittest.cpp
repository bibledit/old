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


using namespace std;


void test_filters ()
{
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
    // Test string modifiers.
    evaluate ("filter_string_trim 1", "", filter_string_trim ("  "));
    evaluate ("filter_string_trim 2", "", filter_string_trim (""));
    evaluate ("filter_string_trim 3", "xx", filter_string_trim ("\t\nxx\n\r"));
    evaluate ("filter_string_fill", "0000012345", filter_string_fill ("12345", 10, '0'));
  }
  {
    // Test URL decoder.
    evaluate ("filter_url_urldecode 1", "Store settings", filter_url_urldecode ("Store+settings"));
    evaluate ("filter_url_urldecode 2", "test@mail", filter_url_urldecode ("test%40mail"));
    evaluate ("filter_url_urldecode 3", "ᨀab\\d@a", filter_url_urldecode ("%E1%A8%80ab%5Cd%40a"));
  }
  {
    // Test dirname and basename functions.
    evaluate ("get_dirname 1", ".", get_dirname (""));
    evaluate ("get_dirname 2", ".", get_dirname ("/"));
    evaluate ("get_dirname 3", ".", get_dirname ("dir/"));
    evaluate ("get_dirname 4", ".", get_dirname ("/dir"));
    evaluate ("get_dirname 5", "foo", get_dirname ("foo/bar"));
    evaluate ("get_dirname 6", "/foo", get_dirname ("/foo/bar"));
    evaluate ("get_dirname 7", "/foo", get_dirname ("/foo/bar/"));
    evaluate ("get_basename 1", "a.txt", get_basename ("/a.txt"));
    evaluate ("get_basename 2", "txt", get_basename ("/txt/"));
    evaluate ("get_basename 3", "foo.bar", get_basename ("/path/to/foo.bar"));
    evaluate ("get_basename 4", "foo.bar", get_basename ("foo.bar"));
  }
  {
    // Test the date and time related functions.
    int month = filter_string_date_numerical_month ();
    if ((month < 1) || (month > 12)) evaluate ("filter_string_date_numerical_month", "current month", convert_to_string (month));
    int year = filter_string_date_numerical_year ();
    if ((year < 2014) || (year > 2050)) evaluate ("filter_string_date_numerical_year", "current year", convert_to_string (year));
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int reference_second = tv.tv_sec;
    int actual_second = filter_string_date_seconds_since_epoch ();
    if (abs (actual_second - reference_second) > 1) evaluate ("filter_string_date_seconds_since_epoch", reference_second, actual_second);
    int usecs = filter_string_date_numerical_microseconds ();
    if ((usecs < 0) || (usecs > 1000000)) evaluate ("filter_string_date_numerical_microseconds", "0-1000000", convert_to_string (usecs));
  }
  {
    evaluate ("filter_string_is_numeric 1", true, filter_string_is_numeric ("1"));
    evaluate ("filter_string_is_numeric 2", true, filter_string_is_numeric ("1234"));
    evaluate ("filter_string_is_numeric 3", false, filter_string_is_numeric ("X"));
    evaluate ("filter_string_is_numeric 4", false, filter_string_is_numeric ("120X"));
  }
  {
    evaluate ("convert_to_int 1", 0, convert_to_int (""));
    evaluate ("convert_to_int 2", 123, convert_to_int ("123"));
    evaluate ("convert_to_int 3", 123, convert_to_int ("123xx"));
    evaluate ("convert_to_int 4", 0, convert_to_int ("xxx123xx"));
  }
  // Test the USFM filter functions. C++Port Todo
  {
    evaluate ("usfm_one_string 1", "", usfm_one_string (""));
    evaluate ("usfm_one_string 2", "\\id GEN", usfm_one_string ("\\id GEN\n"));
    evaluate ("usfm_one_string 3", "\\v 10 text", usfm_one_string ("\\v 10\ntext"));
    evaluate ("usfm_one_string 4", "\\v 10\\v 11", usfm_one_string ("\\v 10\n\\v 11"));
    evaluate ("usfm_one_string 5", "\\v 10 text\\p\\v 11", usfm_one_string ("\\v 10 text\n\\p\\v 11"));
  }
  {
    evaluate ("usfm_get_markers_and_text 1", { "\\id ", "GEN", "\\c ", "10" }, usfm_get_markers_and_text ("\\id GEN\\c 10"));
    evaluate ("usfm_get_markers_and_text 2", { "noise", "\\id ", "GEN", "\\c ", "10" }, usfm_get_markers_and_text ("noise\\id GEN\\c 10"));
    evaluate ("usfm_get_markers_and_text 3", { "\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*" }, usfm_get_markers_and_text ("\\p\\v 1 In \\add the\\add*"));
    evaluate ("usfm_get_markers_and_text 4", { "\\v ", "2 Text ", "\\add ", "of the ", "\\add*", "1st", "\\add ", "second verse", "\\add*", "." }, usfm_get_markers_and_text ("\\v 2 Text \\add of the \\add*1st\\add second verse\\add*."));
    evaluate ("usfm_get_markers_and_text 5", { "\\p", "\\v ", "1 In ", "\\+add ", "the", "\\+add*" }, usfm_get_markers_and_text ("\\p\\v 1 In \\+add the\\+add*"));
  }
  {
    evaluate ("usfm_get_marker 1", "", usfm_get_marker (""));
    evaluate ("usfm_get_marker 2", "id", usfm_get_marker ("\\id GEN"));
    evaluate ("usfm_get_marker 3", "add", usfm_get_marker ("\\add insertion"));
    evaluate ("usfm_get_marker 4", "add", usfm_get_marker ("\\add"));
    evaluate ("usfm_get_marker 5", "add", usfm_get_marker ("\\add*"));
    evaluate ("usfm_get_marker 6", "add", usfm_get_marker ("\\add*\\add"));
    evaluate ("usfm_get_marker 7", "add", usfm_get_marker ("\\+add"));
    evaluate ("usfm_get_marker 8", "add", usfm_get_marker ("\\+add*"));
  }
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Books database_books = Database_Books ();
    database_books.create ();

    evaluate ("usfm_import 1", 0, usfm_import ("", "Standard").size());
    vector <BookChapterData> import2 = usfm_import ("\\id MIC\n\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", "Standard");
    evaluate ("usfm_import 2", 2, import2.size());
    if (import2.size () == 2) {
      evaluate ("usfm_import 3", 33, import2 [0].book);
      evaluate ("usfm_import 4", 0, import2 [0].chapter);
      evaluate ("usfm_import 5", "\\id MIC", import2 [0].data);
      evaluate ("usfm_import 6", 33, import2 [1].book);
      evaluate ("usfm_import 7", 1, import2 [1].chapter);
      evaluate ("usfm_import 8", "\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", import2 [1].data);
    } else evaluate ("usfm_import 9", "executing tests", "skipping tests");

    evaluate ("usfm_get_verse_numbers 1", {0, 1, 2}, usfm_get_verse_numbers ("\\v 1 test\\v 2 test"));

    string usfm = "\\id MIC";
    evaluate ("usfm_linenumber2versenumber 1", 0, usfm_linenumber_to_versenumber (usfm, 0));
    usfm = "\\id MIC\n\\v 1 Verse";
    evaluate ("usfm_linenumber2versenumber 2", 1, usfm_linenumber_to_versenumber (usfm, 1));
    usfm = "\\v 1 Verse";
    evaluate ("usfm_linenumber2versenumber 3", 1, usfm_linenumber_to_versenumber (usfm, 0));
    usfm = "\\p\n\\v 3 Verse 3 (out of order).\n\\v 1 Verse 1. \n\\v 2 Verse 1.";
    evaluate ("usfm_linenumber2versenumber 4", 0, usfm_linenumber_to_versenumber (usfm, 0));
    evaluate ("usfm_linenumber2versenumber 5", 3, usfm_linenumber_to_versenumber (usfm, 1));
    evaluate ("usfm_linenumber2versenumber 6", 1, usfm_linenumber_to_versenumber (usfm, 2));
    evaluate ("usfm_linenumber2versenumber 7", 2, usfm_linenumber_to_versenumber (usfm, 3));


/* Todo
  

*/
  }
}


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
    evaluate ("Database_Bibles::getBibles 1", standard, bibles);
  }
  {
    // Test whether optimizing works without errors.
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    int id = database_bibles.createBible ("phpunit");
    if (id == 0) evaluate ("Database_Bibles::createBible", "non-zero", id);
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "g");
    database_bibles.optimize ();
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate ("Database_Bibles::optimize", "g", usfm);
  }
  {
    // Test whether optimizing removes files with 0 size.
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    int id = database_bibles.createBible ("phpunit");
    if (id == 0) evaluate ("Database_Bibles::createBible", "non-zero", id);
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "");
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate ("Database_Bibles::getChapter 2", "", usfm);
    database_bibles.optimize ();
    usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate ("Database_Bibles::optimize 2", "f", usfm);
  }
  // Test create / get / delete Bibles.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    int id = database_bibles.createBible ("phpunit");
    evaluate ("Database_Bibles::createBible 3", 1, id);
    
    vector <string> bibles = database_bibles.getBibles ();
    vector <string> standard = {"phpunit"};
    evaluate ("Database_Bibles::getBibles 3", standard, bibles);

    id = database_bibles.getID ("phpunit2");
    evaluate ("Database_Bibles::getID 3", 0, id);
    
    database_bibles.deleteBible ("phpunit");

    id = database_bibles.getID ("phpunit");
    evaluate ("Database_Bibles::deleteBible 3", 0, id);
  }
  // Test names / identifiers.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    int id = database_bibles.getID ("phpunit");
    evaluate ("Database_Bibles::getID 4", 0, id);

    string bible = database_bibles.getName (0);
    evaluate ("Database_Bibles::getName 4", "Unknown", bible);

    id = database_bibles.createBible ("phpunit");
    evaluate ("Database_Bibles::createBible 4", 1, id);

    id = database_bibles.getID ("phpunit");
    evaluate ("Database_Bibles::getID 5", 1, id);

    bible = database_bibles.getName (1);
    evaluate ("Database_Bibles::getName 5", "phpunit", bible);
    
    bible = database_bibles.getName (2);
    evaluate ("Database_Bibles::getName 5", "Unknown", bible);
  }
  // Test storeChapter / getChapter
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    string usfm = "\\c 1\n\\p\n\\v 1 Verse 1";
    database_bibles.storeChapter ("phpunit", 2, 1, usfm);
    string result = database_bibles.getChapter ("phpunit", 2, 1);
    evaluate ("Database_Bibles::storeChapter getChapter 1", usfm, result);
    result = database_bibles.getChapter ("phpunit2", 2, 1);
    evaluate ("Database_Bibles::storeChapter getChapter 2", "", result);
    result = database_bibles.getChapter ("phpunit", 1, 1);
    evaluate ("Database_Bibles::storeChapter getChapter 3", "", result);
  }
  // Test books
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    vector <int> books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::getBooks 6", { }, books);

    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::storeChapter getBooks 6", { 1 }, books);

    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 0");
    books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::storeChapter getBooks 7", { 1, 2 }, books);

    database_bibles.deleteBook ("phpunit", 3);
    books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::storeChapter deleteBook 6", { 1, 2 }, books);

    database_bibles.deleteBook ("phpunit", 1);
    books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::storeChapter deleteBook 7", { 2 }, books);

    database_bibles.deleteBook ("phpunit2", 2);
    books = database_bibles.getBooks ("phpunit");
    evaluate ("Database_Bibles::storeChapter deleteBook 8", { 2 }, books);
  }
  // Test chapters ()
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    database_bibles.createBible ("phpunit");
    vector <int> chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 1", { }, chapters);
 
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 2", { 2 }, chapters);
    
    chapters = database_bibles.getChapters ("phpunit", 2);
    evaluate ("Database_Bibles::test chapters 3", { }, chapters);

    database_bibles.storeChapter ("phpunit", 1, 3, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 4", { 2, 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 3, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 5", { 2, 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 1, 2);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 6", { 3 }, chapters);

    database_bibles.deleteChapter ("phpunit", 1, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate ("Database_Bibles::test chapters 7", { }, chapters);
  }
  // Test chapter IDs
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();

    database_bibles.createBible ("phpunit");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    int id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate ("Database_Bibles::chapter IDs 1", 100000001, id);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate ("Database_Bibles::chapter IDs 2", 100000002, id);

    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate ("Database_Bibles::chapter IDs 3", 100000004, id);

    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate ("Database_Bibles::chapter IDs 4", 100000004, id);
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


// Tests for Database_Search
void test_database_search_setup ()
{
  string standardUSFM1 =  "\\c 1"
                          "\\p"
                          "\\v 1 Text of the first verse."
                          "\\v 2 \\add Text of the second \\add*verse."
                          "\\s Heading"
                          "\\p"
                          "\\v 3 Text of the 3rd third verse."
                          "\\v 4 Text of the 4th \\add fourth\\add* verse."
                          "\\v 5 Text of the 5th fifth verse is this: Verse five ✆."
                          "\\v 6 Text of the 6th sixth verse ✆."
                          "\\v 7 Text of the seventh verse with some UTF-8 characters: ✆ ❼ ሯ."
                          "\\v 8 Verse eight ✆ ❼ ሯ."
                          "\\v 9 Verse nine nine."
                          "\\v 10 خدا بود و کلمه کلمه خدا بود. در ابتدا کلمه بود و کلمه نزد.";
  string standardUSFM2 =  "\\c 1"
                          "\\p"
                          "\\v 3 And he said.";
  Database_Search database_search = Database_Search ();
  database_search.create ();
  Database_Bibles database_bibles = Database_Bibles ();
  database_bibles.createBible ("phpunit");
  database_bibles.storeChapter ("phpunit", 2, 3, standardUSFM1);
  database_bibles.createBible ("phpunit2");
  database_bibles.storeChapter ("phpunit2", 4, 5, standardUSFM2);
}


void test_database_search ()
{
  // Test create, optimize, health.
  {
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();

    database_search.optimize ();
    bool healthy = database_search.checkup ();
    evaluate ("Database_Search optimize", false, healthy);

    string path = filter_url_create_root_path ("databases", "search.sqlite");
    filter_url_file_put_contents (path, "damaged database");
    healthy = database_search.checkup ();
    evaluate ("Database_Search checkup 1", true, healthy);
    healthy = database_search.checkup ();
    evaluate ("Database_Search checkup 2", false, healthy);
    refresh_sandbox (false);
  }
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    // Test Update Fields
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
  }
/* C++Port
  public function testSearchGetBiblePassage ()
  {
    database_search = Database_Search::getInstance ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    $hits = database_search.searchText ("sixth", array ("phpunit"));
    $this.assertEquals (1, count ($hits));
    $id = $hits [0];
    $data = database_search.getBiblePassage ($id);
    $this.assertEquals ("phpunit", $data ['bible']);
    $this.assertEquals (2, $data ['book']);
    $this.assertEquals (3, $data ['chapter']);
    $this.assertEquals (6, $data ['verse']);
  }
  
  
  public function testGetBibleVerseText ()
  {
    database_search = Database_Search::getInstance ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    $text = database_search.getBibleVerseText ("phpunit", 2, 3, 5);
    $this.assertEquals ("Text of the 5th fifth verse is this: Verse five ✆.", trim ($text));
  }


  public function testSearchBible ()
  {
    database_search = Database_Search::getInstance ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    $ids = database_search.searchBibleText ("phpunit", "sixth");
    $this.assertEquals (1, count ($ids));
    $ids = database_search.searchBibleText ("phpunit2", "sixth");
    $this.assertEquals (0, count ($ids));
    $ids = database_search.searchBibleText ("phpunit2", "said");
    $this.assertEquals (1, count ($ids));
  }


  public function testSearchBibleCaseSensitive ()
  {
    database_search = Database_Search::getInstance ();
    database_search.updateSearchFields ("phpunit", 2, 1);
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    $ids = database_search.searchBibleTextCaseSensitive ("phpunit", "Verse");
    $this.assertEquals (3, count ($ids));
    $ids = database_search.searchBibleText ("phpunit", "sixth");
    $this.assertEquals (1, count ($ids));
    $ids = database_search.searchBibleText ("phpunit2", "said");
    $this.assertEquals (1, count ($ids));
  }


  public function testGetBibles ()
  {
    database_search = Database_Search::getInstance ();
    $bibles = database_search.getBibles ();
    $this.assertContains ("phpunit", $bibles);
    $this.assertContains ("phpunit2", $bibles);
    database_search.deleteBible ("phpunit");
    $bibles = database_search.getBibles ();
    $this.assertContains ("phpunit2", $bibles);
  }


  public function testGetBooks ()
  {
    database_search = Database_Search::getInstance ();
    $books = database_search.getBooks ("phpunit");
    $this.assertContains (2, $books);
    database_search.deleteBook ("phpunit", 2);
    $books = database_search.getBooks ("phpunit");
    $this.assertNotContains (2, $books);
  }


  public function testGetChapters ()
  {
    database_search = Database_Search::getInstance ();
    $chapters = database_search.getChapters ("phpunit", 2);
    $this.assertContains (3, $chapters);
    database_search.deleteChapter ("phpunit", 2, 3);
    $chapters = database_search.getChapters ("phpunit", 2);
    $this.assertNotContains (3, $chapters);
  }


  public function testGetVerses ()
  {
    database_search = Database_Search::getInstance ();
    $verses = database_search.getVerses ("phpunit", 2, 3);
    $standard = array ();
    for ($i = 0; $i <= 10; $i++) $standard [] = $i;
    $this.assertEquals ($standard, $verses);
  }


  public function testGetVerseCount ()
  {
    database_search = Database_Search::getInstance ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    $count = database_search.getVerseCount ("phpunit");
    $this.assertEquals (11, $count);
  }
*/
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

test_filters (); exit (0); // Todo

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


