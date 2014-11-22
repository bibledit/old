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


#include <unittests/databases.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <config/globals.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <database/search.h>


// Tests for Database_Search
void test_database_search_setup ()
{
  Database_Styles database_styles = Database_Styles ();
  database_styles.create ();
  database_styles.createStandardSheet ();

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
    evaluate (__LINE__, __func__, false, healthy);

    string path = filter_url_create_root_path ("databases", "search.sqlite");
    filter_url_file_put_contents (path, "damaged database");
    healthy = database_search.checkup ();
    evaluate (__LINE__, __func__, true, healthy);
    healthy = database_search.checkup ();
    evaluate (__LINE__, __func__, false, healthy);
    refresh_sandbox (false);
  }
/* C++Port
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    // Test Update Fields
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
  }

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


