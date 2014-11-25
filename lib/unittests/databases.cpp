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


void test_database_styles ()
{
  // Tests for Database_Styles.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();

    vector <string> sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { "Standard" }, sheets);

    database_styles.createSheet ("phpunit");
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { "Standard", "phpunit" }, sheets);

    database_styles.deleteSheet ("phpunit");
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { "Standard" }, sheets);

    database_styles.deleteSheet ("Standard");
    sheets = database_styles.getSheets ();
    evaluate (__LINE__, __func__, { "Standard" }, sheets);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");

    vector <string> markers;
    
    markers = database_styles.getMarkers ("Standard");
    evaluate (__LINE__, __func__, 179, markers.size ());

    markers = database_styles.getMarkers ("phpunit");
    evaluate (__LINE__, __func__, 179, markers.size ());

    string marker = "p";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");

    map <string, string> markers_names = database_styles.getMarkersAndNames ("phpunit");
    evaluate (__LINE__, __func__, 179, markers_names.size());
    evaluate (__LINE__, __func__, "Blank Line", markers_names ["b"]);
    evaluate (__LINE__, __func__, "Normal, First Line Indent", markers_names ["p"]);
    evaluate (__LINE__, __func__, "* Translational Addition", markers_names ["add"]);

    database_styles.deleteMarker ("phpunit", "p");
    markers = database_styles.getMarkers ("phpunit");
    marker = "p";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate (__LINE__, __func__, marker, "should not be there");
    marker = "add";
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "not found");

    markers_names = database_styles.getMarkersAndNames ("phpunit");
    evaluate (__LINE__, __func__, "", markers_names ["p"]);
    evaluate (__LINE__, __func__, "* Translational Addition", markers_names ["add"]);    
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate (__LINE__, __func__, "add", data.marker);
    evaluate (__LINE__, __func__, "st", data.category);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    database_styles.updateName ("phpunit", "add", "Addition");
    Database_Styles_Item data = database_styles.getMarkerData ("phpunit", "add");
    evaluate (__LINE__, __func__, "Addition", data.name);
    database_styles.updateInfo ("phpunit", "p", "Paragraph");
    data = database_styles.getMarkerData ("phpunit", "p");
    evaluate (__LINE__, __func__, "Paragraph", data.info);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // A user does not have write access to the stylesheet.
    bool write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, false, write);

    // Grant write access, and test it for this user, and for another user.
    database_styles.grantWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, true, write);
    write = database_styles.hasWriteAccess ("user2", "phpunit");
    evaluate (__LINE__, __func__, false, write);
    write = database_styles.hasWriteAccess ("user", "phpunit2");
    evaluate (__LINE__, __func__, false, write);

    // Revoke write access for a user, test it in various ways.
    database_styles.revokeWriteAccess ("user2", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, true, write);
    database_styles.revokeWriteAccess ("user", "phpunit");
    write = database_styles.hasWriteAccess ("user", "phpunit");
    evaluate (__LINE__, __func__, false, write);
    
    // Revoking write access for all users.
    database_styles.grantWriteAccess ("user1", "phpunit");
    database_styles.grantWriteAccess ("user2", "phpunit");
    database_styles.revokeWriteAccess ("", "phpunit");
    write = database_styles.hasWriteAccess ("user1", "phpunit");
    evaluate (__LINE__, __func__, false, write);
  }
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    database_styles.createSheet ("phpunit");

    // Get markers.
    vector <string> markers = database_styles.getMarkers ("phpunit");
    string marker = "zhq";
    if (find (markers.begin (), markers.end (), marker) != markers.end ()) evaluate (__LINE__, __func__, marker, "should not be there");
  
    // Add marker.
    database_styles.addMarker ("phpunit", marker);
    markers = database_styles.getMarkers ("phpunit");
    if (find (markers.begin (), markers.end (), marker) == markers.end ()) evaluate (__LINE__, __func__, marker, "should be there");
  }
}


// Tests for the Database_Books object.
void test_database_books ()
{
  refresh_sandbox (true);
  evaluate (__LINE__, __func__, 69, Database_Books::getIDs ().size());
  evaluate (__LINE__, __func__, 2, Database_Books::getIdFromEnglish ("Exodus"));
  evaluate (__LINE__, __func__, 0, Database_Books::getIdFromEnglish ("exodus"));
  evaluate (__LINE__, __func__, "Leviticus", Database_Books::getEnglishFromId (3));
  evaluate (__LINE__, __func__, "NUM", Database_Books::getUsfmFromId (4));
  evaluate (__LINE__, __func__, "Deu", Database_Books::getBibleworksFromId (5));
  evaluate (__LINE__, __func__, 22, Database_Books::getIdFromUsfm ("SNG"));
  evaluate (__LINE__, __func__, 13, Database_Books::getIdFromOsis ("1Chr"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdFromBibleworks ("2Ki"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdLikeText ("2Ki"));
  evaluate (__LINE__, __func__, 12, Database_Books::getIdFromOnlinebible ("2Ki"));
  evaluate (__LINE__, __func__, "De", Database_Books::getOnlinebibleFromId (5));
  evaluate (__LINE__, __func__, "5", Database_Books::getSequenceFromId (5));
  evaluate (__LINE__, __func__, "nt", Database_Books::getType (40));
  evaluate (__LINE__, __func__, "ot", Database_Books::getType (39));
  evaluate (__LINE__, __func__, "", Database_Books::getType (0));
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


