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
#include <database/bibleactions.h>
#include <database/check.h>
#include <database/commits.h>
#include <database/confirm.h>
#include <database/history.h>
#include <database/ipc.h>
#include <database/jobs.h>
#include <database/kjv.h>
#include <database/morphhb.h>
#include <database/sblgnt.h>
#include <database/offlineresources.h>
#include <database/sprint.h>
#include <database/mail.h>
#include <database/navigation.h>


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
  // Test updating search fields.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
  }
  // Test searching and getting Bible passage
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    vector <int> hits = database_search.searchText ("sixth", {"phpunit"});
    evaluate (__LINE__, __func__, 1, hits.size());
    if (!hits.empty ()) {
      int id = hits [0];
      Passage passage = database_search.getBiblePassage (id);
      evaluate (__LINE__, __func__, "phpunit", passage.bible);
      evaluate (__LINE__, __func__, 2, passage.book);
      evaluate (__LINE__, __func__, 3, passage.chapter);
      evaluate (__LINE__, __func__, "6", passage.verse);
    }
  }
  // Test getting Bible verse text.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    string text = database_search.getBibleVerseText ("phpunit", 2, 3, 5);
    evaluate (__LINE__, __func__, "Text of the 5th fifth verse is this: Verse five ✆.", filter_string_trim (text));
  }
  // Test search Bible
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    vector <int> ids = database_search.searchBibleText ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "sixth");
    evaluate (__LINE__, __func__, 0, ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, ids.size ());
  }
  // Test search Bible case sensitive.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 1);
    database_search.updateSearchFields ("phpunit", 2, 3);
    database_search.updateSearchFields ("phpunit2", 4, 5);
    vector <int> ids = database_search.searchBibleTextCaseSensitive ("phpunit", "Verse");
    evaluate (__LINE__, __func__, 3, ids.size ());
    ids = database_search.searchBibleText ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, ids.size ());
    ids = database_search.searchBibleText ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, ids.size ());
  }
  // Test getting Bibles.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <string> bibles = database_search.getBibles ();
    evaluate (__LINE__, __func__, { "phpunit", "phpunit2" }, bibles);
    database_search.deleteBible ("phpunit");
    bibles = database_search.getBibles ();
    evaluate (__LINE__, __func__, { "phpunit2" }, bibles);
  }
  // Test getting books.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> books = database_search.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);
    database_search.deleteBook ("phpunit", 2);
    books = database_search.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { }, books);
  }
  // Test getting chapters.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> chapters = database_search.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { 3 }, chapters);
    database_search.deleteChapter ("phpunit", 2, 3);
    chapters = database_search.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { }, chapters);
  }
  // Test getVerses.
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    vector <int> verses = database_search.getVerses ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, verses);
  }
  // Test getVerseCount ()
  {
    refresh_sandbox (true);
    test_database_search_setup ();
    Database_Search database_search = Database_Search ();
    database_search.updateSearchFields ("phpunit", 2, 3);
    int count = database_search.getVerseCount ("phpunit");
    evaluate (__LINE__, __func__, 11, count);
  }
}


void test_database_bibleactions ()
{
  refresh_sandbox (true);
  Database_BibleActions database_bibleactions = Database_BibleActions ();
  database_bibleactions.create ();
  
  database_bibleactions.optimize ();
  
  vector <string> bibles = database_bibleactions.getBibles ();
  evaluate (__LINE__, __func__, { }, bibles);

  database_bibleactions.record ("phpunit1", 1, 2, "data112");
  database_bibleactions.record ("phpunit1", 1, 3, "data113");
  database_bibleactions.record ("phpunit1", 2, 4, "data124");
  database_bibleactions.record ("phpunit2", 5, 6, "data256");
  database_bibleactions.record ("phpunit2", 5, 6, "data256: Not to be stored");

  bibles = database_bibleactions.getBibles ();
  evaluate (__LINE__, __func__, {"phpunit1", "phpunit2"}, bibles);

  vector <int> books = database_bibleactions.getBooks ("phpunit1");
  evaluate (__LINE__, __func__, {1, 2}, books);

  vector <int> chapters = database_bibleactions.getChapters ("phpunit1", 1);
  evaluate (__LINE__, __func__, {2, 3}, chapters);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, {4}, chapters);
  
  database_bibleactions.erase ("phpunit1", 2, 3);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, {4}, chapters);
  
  database_bibleactions.erase ("phpunit1", 2, 4);
  
  chapters = database_bibleactions.getChapters ("phpunit1", 2);
  evaluate (__LINE__, __func__, { }, chapters);
  
  string usfm = database_bibleactions.getUsfm ("phpunit2", 5, 5);
  evaluate (__LINE__, __func__, "", usfm);
  
  usfm = database_bibleactions.getUsfm ("phpunit2", 5, 6);
  evaluate (__LINE__, __func__, "data256", usfm);
}


void test_database_check ()
{
  {
    // Test Optimize
    refresh_sandbox (true);
    Database_Check database_check = Database_Check ();
    database_check.optimize ();
  }
  {
    // Test Record Get Truncate.
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();

    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, hits.size());
  
    database_check.recordOutput ("phpunit", 1, 2, 3, "test");
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, hits.size());
    
    database_check.truncateOutput ("");
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, hits.size());
  }
  {
    // Test getting details.
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 5, 2, 3, "test");
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, hits.size());
    evaluate (__LINE__, __func__, 1, hits [0].bible);
    evaluate (__LINE__, __func__, 5, hits [0].book);
    evaluate (__LINE__, __func__, 2, hits [0].chapter);
    evaluate (__LINE__, __func__, "test", hits [0].data);
  }
  {
    // Test approvals.
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();

    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 3, 4, 5, "test2");
  
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, hits.size());
  
    int id = hits [0].rowid;
    database_check.approve (id);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, hits.size());
    
    vector <Database_Check_Hit> suppressions = database_check.getSuppressions ();
    evaluate (__LINE__, __func__, 1, suppressions.size());
    
    id = suppressions [0].rowid;
    
    database_check.release (1);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, hits.size());
  }
  {
    // Test delete.
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 3, 4, 5, "test2");
    vector <Database_Check_Hit> hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, hits.size());
    int id = hits [0].rowid;
    database_check.erase (id);
    hits = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, hits.size());
  }
  {
    // Test passage.
    refresh_sandbox (true);
    Database_Search database_search = Database_Search ();
    database_search.create ();
    Database_Bibles database_bibles = Database_Bibles ();
    database_bibles.createBible ("phpunit");
    Database_Check database_check = Database_Check ();
    database_check.create ();
    database_check.recordOutput ("phpunit", 3, 4, 5, "test1");
    database_check.recordOutput ("phpunit", 6, 7, 8, "test2");
    Passage passage = database_check.getPassage (2);
    evaluate (__LINE__, __func__, 6, passage.book);
    evaluate (__LINE__, __func__, 7, passage.chapter);
    evaluate (__LINE__, __func__, "8", passage.verse);
  }
}


void test_database_commits ()
{
  refresh_sandbox (true);
  Database_Commits database_commits = Database_Commits ();
  database_commits.create ();
  database_commits.optimize ();

  string bible = "phpunit";
  string sha = "sha";

  vector <string> data = database_commits.get (bible);
  evaluate (__LINE__, __func__, 0, data.size());
  
  // Record some data.
  database_commits.record (bible, sha);
  database_commits.record (bible, sha);

  // Check the data.
  data = database_commits.get (bible);
  evaluate (__LINE__, __func__, 2, data.size());
  evaluate (__LINE__, __func__, "sha", data [1]);

  // No data for another Bible
  data = database_commits.get ("none");
  evaluate (__LINE__, __func__, 0, data.size());
}


void test_database_confirm ()
{
  refresh_sandbox (true);
  Database_Confirm database_confirm = Database_Confirm ();
  database_confirm.create ();

  database_confirm.optimize ();
  database_confirm.trim ();

  // New ID generation test.
  unsigned int id = database_confirm.getNewID ();
  if (id < 10000) evaluate (__LINE__, __func__, "Should be greater than 10000", id);
  
  // Store data for the ID.
  database_confirm.store (id, "SELECT x, y, z FROM a;", "email", "subject", "body");
  
  // Search for this ID based on subject.
  unsigned int id2 = database_confirm.searchID ("Subject line CID" + convert_to_string (id) + " Re:");
  evaluate (__LINE__, __func__, id, id2);

  // Retrieve data for the ID.
  string query = database_confirm.getQuery (id);
  evaluate (__LINE__, __func__,"SELECT x, y, z FROM a;", query);

  string to = database_confirm.getMailTo (id);
  evaluate (__LINE__, __func__,"email", to);

  string subject = database_confirm.getSubject (id);
  evaluate (__LINE__, __func__,"subject", subject);

  string body = database_confirm.getBody (id);
  evaluate (__LINE__, __func__,"body", body);

  // Delete this ID.
  database_confirm.erase (id);
  query = database_confirm.getQuery (id);
  evaluate (__LINE__, __func__,"", query);
}


void test_database_history ()
{
  refresh_sandbox (true);
  Database_History database_history = Database_History ();
  database_history.create ();
  database_history.optimize ();
  database_history.trim ();
  
  string author = "test";
  string bible = "phpunit";
  int book = 1;
  int chapter = 2;
  int verse = 3;
  int start = 0;

  // Start with an empty history.
  int count = database_history.count (author, {bible}, book, chapter, verse);
  evaluate (__LINE__, __func__, 0, count);

  count = database_history.count ("", {}, 0, -1, -1);
  evaluate (__LINE__, __func__, 0, count);

  vector <Database_History_Item> data = database_history.get (author, {bible}, book, chapter, verse, start);
  evaluate (__LINE__, __func__, 0, data.size ());

  vector <string> authors = database_history.authors ({bible});
  evaluate (__LINE__, __func__, 0, authors.size ());

  // Record some data.
  database_history.record (filter_string_date_seconds_since_epoch(), author, bible, book, chapter, verse, "old1", "mod1", "new1");
  database_history.record (filter_string_date_seconds_since_epoch(), author, bible, book, chapter, verse, "old2", "mod2", "new2");

  // Check the data.
  count = database_history.count (author, {bible}, book, chapter, verse);
  evaluate (__LINE__, __func__, 2, count);
  
  count = database_history.count ("", {}, -1, -1, -1);
  evaluate (__LINE__, __func__, 2, count);

  data = database_history.get (author, {bible}, book, chapter, verse, start);
  evaluate (__LINE__, __func__, 2, data.size ());
  evaluate (__LINE__, __func__, "test", data [0].author);
  evaluate (__LINE__, __func__, "new2", data [1].newtext);
}


void test_database_ipc ()
{
   // Test Trim
  {
    refresh_sandbox (true);
    Database_Ipc database_ipc = Database_Ipc (NULL);
    database_ipc.trim ();
  }
  // Test Store Retrieve
  {
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    int id = 1;
    string user = "phpunit";
    string channel = "channel";
    string command = "command";
    string message = "message";
  
    database_ipc.storeMessage (user, channel, command, message);
   
    Database_Ipc_Message data = database_ipc.retrieveMessage (id, user, channel, command);
    evaluate (__LINE__, __func__, 0, data.id);
  
    database_ipc.storeMessage (user, channel, command, message);
  
    data = database_ipc.retrieveMessage (id, user, channel, command);
    evaluate (__LINE__, __func__, 2, data.id);
    evaluate (__LINE__, __func__, message, data.message);
  }
  // Test Delete
  {
    refresh_sandbox (true);
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    int id = 1;
    string user = "phpunit";
    string channel = "channel";
    string command = "command";
    string message = "message";
    
    database_ipc.storeMessage (user, channel, command, message);
    
    Database_Ipc_Message data = database_ipc.retrieveMessage (0, user, channel, command);
    evaluate (__LINE__, __func__, id, data.id);
    
    database_ipc.deleteMessage (id);
  
    data = database_ipc.retrieveMessage (0, user, channel, command);
    evaluate (__LINE__, __func__, 0, data.id);
  }
  // Test Get Focus
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);

    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "focus";
  
    string passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, "1.1.1", passage);
  
    string message = "2.3.4";
    database_ipc.storeMessage (user, channel, command, message);
    passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, message, passage);
  
    message = "5.6.7";
    database_ipc.storeMessage (user, channel, command, message);
    passage = database_ipc.getFocus ();
    evaluate (__LINE__, __func__, message, passage);
  }
  // Test Get Note.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);
    
    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "opennote";
  
    Database_Ipc_Message note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, 0, note.id);
  
    string message = "12345";
    database_ipc.storeMessage (user, channel, command, message);
    note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, message, note.message);
  
    message = "54321";
    database_ipc.storeMessage (user, channel, command, message);
    note = database_ipc.getNote ();
    evaluate (__LINE__, __func__, message, note.message);
  }
  // Test Notes Alive.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);
    
    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "notesalive";
  
    bool alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, false, alive);
  
    string message = "1";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  
    message = "0";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getNotesAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  }
  // Test Bible Alive.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Ipc database_ipc = Database_Ipc (&request);
    
    string user = "phpunit";
    request.session_logic ()->setUsername (user);
    string channel = "channel";
    string command = "biblealive";
  
    bool alive = database_ipc.getBibleAlive ();
    evaluate (__LINE__, __func__, false, alive);
  
    string message = "1";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getBibleAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  
    message = "0";
    database_ipc.storeMessage (user, channel, command, message);
    alive = database_ipc.getBibleAlive ();
    evaluate (__LINE__, __func__, convert_to_bool (message), alive);
  }
}


void test_database_jobs ()
{
  {
    refresh_sandbox (true);
    Database_Jobs database_jobs = Database_Jobs ();
    database_jobs.create ();

    // Test Optimize
    database_jobs.optimize ();

    // Test Identifiers
    int id = database_jobs.getNewId ();
    bool exists = database_jobs.idExists (id);
    evaluate (__LINE__, __func__, true, exists);

    // Test Level.
    id = database_jobs.getNewId ();
    int level = database_jobs.getLevel (id);
    evaluate (__LINE__, __func__, 0, level);
    database_jobs.setLevel (id, 123);
    level = database_jobs.getLevel (id);
    evaluate (__LINE__, __func__, 123, level);

    // Test Progress
    id = database_jobs.getNewId ();
    string progress = database_jobs.getProgress (id);
    evaluate (__LINE__, __func__, "", progress);
    database_jobs.setProgress (id, "progress");
    progress = database_jobs.getProgress (id);
    evaluate (__LINE__, __func__, "progress", progress);

    // Test Result.
    id = database_jobs.getNewId ();
    string result = database_jobs.getResult (id);
    evaluate (__LINE__, __func__, "", result);
    database_jobs.setResult (id, "result");
    result = database_jobs.getResult (id);
    evaluate (__LINE__, __func__, "result", result);
  }
}


void test_database_kjv ()
{
  Database_Kjv database_kjv = Database_Kjv ();

  vector <Database_Kjv_Item> data = database_kjv.getVerse (43, 11, 35);
  evaluate (__LINE__, __func__, 3, data.size());

  evaluate (__LINE__, __func__, "G3588", data[0].strong);
  evaluate (__LINE__, __func__, "Jesus", data[0].english);

  evaluate (__LINE__, __func__, "G2424", data[1].strong);
  evaluate (__LINE__, __func__, "Jesus", data[1].english);

  evaluate (__LINE__, __func__, "G1145", data[2].strong);
  evaluate (__LINE__, __func__, "wept",  data[2].english);

  vector <Passage> passages = database_kjv.searchStrong ("G909");
  evaluate (__LINE__, __func__, 4, passages.size());

  evaluate (__LINE__, __func__, 41,   passages[0].book);
  evaluate (__LINE__, __func__, 7,    passages[0].chapter);
  evaluate (__LINE__, __func__, "4",  passages[0].verse);

  evaluate (__LINE__, __func__, 41,   passages[1].book);
  evaluate (__LINE__, __func__, 7,    passages[1].chapter);
  evaluate (__LINE__, __func__, "8",  passages[1].verse);

  evaluate (__LINE__, __func__, 58,   passages[2].book);
  evaluate (__LINE__, __func__, 6,    passages[2].chapter);
  evaluate (__LINE__, __func__, "2",  passages[2].verse);

  evaluate (__LINE__, __func__, 58,   passages[3].book);
  evaluate (__LINE__, __func__, 9,    passages[3].chapter);
  evaluate (__LINE__, __func__, "10", passages[3].verse);
}


void test_database_morphhb ()
{
  Database_Morphhb database_morphhb = Database_Morphhb ();

  vector <string> data = database_morphhb.getVerse (18, 3, 2);
  evaluate (__LINE__, __func__, { "וַיַּ֥עַן", "אִיּ֗וֹב", "וַיֹּאמַֽר" }, data);

  vector <Passage> passages = database_morphhb.searchHebrew ("יָדְע֥וּ");
  evaluate (__LINE__, __func__, 2, passages.size());

  evaluate (__LINE__, __func__, 19,   passages[0].book);
  evaluate (__LINE__, __func__, 95,   passages[0].chapter);
  evaluate (__LINE__, __func__, "10", passages[0].verse);

  evaluate (__LINE__, __func__, 30,   passages[1].book);
  evaluate (__LINE__, __func__, 3,    passages[1].chapter);
  evaluate (__LINE__, __func__, "10", passages[1].verse);
}


void test_database_sblgnt ()
{
  Database_Sblgnt database_sblgnt = Database_Sblgnt ();

  vector <string> data = database_sblgnt.getVerse (43, 11, 35);
  evaluate (__LINE__, __func__, { "ἐδάκρυσεν", "ὁ", "Ἰησοῦς" }, data);

  vector <Passage> passages = database_sblgnt.searchGreek ("βαπτισμῶν");
  evaluate (__LINE__, __func__, 1,   passages.size());
  evaluate (__LINE__, __func__, 58,  passages[0].book);
  evaluate (__LINE__, __func__, 6,   passages[0].chapter);
  evaluate (__LINE__, __func__, "2", passages[0].verse);
}


void test_database_offlineresourcese ()
{
  // Test Store / Exists / Get.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    bool exists = database_offlineresources.exists ("phpunit", 1, 2, 3);
    evaluate (__LINE__, __func__, true, exists);
    exists = database_offlineresources.exists ("phpunit", 1, 2, 4);
    evaluate (__LINE__, __func__, false, exists);
    string html = database_offlineresources.get ("phpunit", 1, 2, 3);
    evaluate (__LINE__, __func__, "xyz", html);
    html = database_offlineresources.get ("phpunit", 1, 2, 4);
    evaluate (__LINE__, __func__, "", html);
  }
  // Test Count / Delete.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    int count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 0, count);
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 1, count);
    database_offlineresources.store ("phpunit", 4, 5, 6, "xyz");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 2, count);
    database_offlineresources.erase ("phpunit");
    count = database_offlineresources.count ("phpunit");
    evaluate (__LINE__, __func__, 0, count);
  }
  // Test Names.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    vector <string> names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {}, names);
  
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {"phpunit"}, names);
    
    database_offlineresources.store ("phpunit2", 1, 2, 3, "xyz");
    names = database_offlineresources.names ();
    evaluate (__LINE__, __func__, {"phpunit", "phpunit2"}, names);
  }
  // Test Files.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();

    vector <string> files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {}, files);
    
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
  
    database_offlineresources.store ("phpunit", 1, 4, 5, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite"}, files);
  
    database_offlineresources.store ("phpunit", 11, 22, 33, "xyz");
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__, {"1.sqlite", "11.sqlite"}, files);
  }
  // Test Size.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
    int size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 0, size);
  
    database_offlineresources.store ("phpunit", 1, 2, 3, string (100, 's'));
    size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 2048, size);
  
    database_offlineresources.store ("phpunit", 1, 3, 4, string (1000, 's'));
    size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 3072, size);
  }
  // Test Save / Load.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
    
    int size = database_offlineresources.size ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, 0, size);
    
    string standard = "abc";
    database_offlineresources.save ("phpunit", "1.sqlite", standard);
    string contents = database_offlineresources.load ("phpunit", "1.sqlite");
    evaluate (__LINE__, __func__, standard, contents);
  }
  // Test Unlink.
  {
    refresh_sandbox (true);
    Database_OfflineResources database_offlineresources = Database_OfflineResources ();
  
    database_offlineresources.store ("phpunit", 1, 2, 3, "xyz");
    vector <string> files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__,  {"1.sqlite"}, files);
    
    database_offlineresources.unlink ("phpunit", "2");
    
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__,  {"1.sqlite"}, files);
  
    database_offlineresources.unlink ("phpunit", "1.sqlite");
    
    files = database_offlineresources.files ("phpunit");
    evaluate (__LINE__, __func__,  {}, files);
  }
}


void test_database_sprint ()
{
  // Maintenance.
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.optimize ();
  }
  // Store Task / GetTask
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, {}, ids);
    database.storeTask ("phpunit", 2014, 1, "phpunit");
    ids = database.getTasks ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 1, ids.size ());
    ids = database.getTasks ("phpunit", 2014, 2);
    evaluate (__LINE__, __func__, 0, ids.size ());
    ids = database.getTasks ("phpunit2", 2014, 1);
    evaluate (__LINE__, __func__, 0, ids.size ());
  }
  // GetTitle
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.storeTask ("phpunit", 2014, 1, "xyz");
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    string title = database.getTitle (ids[0]);
    evaluate (__LINE__, __func__, "xyz", title);
  }
  // Complete
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();
    database.storeTask ("phpunit", 2014, 1, "phpunit");
    vector <int> ids = database.getTasks ("phpunit", 2014, 1);
    int id = ids[0];
    int complete = database.getComplete (id);
    evaluate (__LINE__, __func__, 0, complete);
    database.updateComplete (id, 95);
    complete = database.getComplete (id);
    evaluate (__LINE__, __func__, 95, complete);
  }
  // History
  {
    refresh_sandbox (true);
    Database_Sprint database = Database_Sprint ();
    database.create ();

    // Expect no history at all for January 2014.
    vector <Database_Sprint_Item> history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 0, history.size());

    // Log values for January 2014, and check that the database returns those values.
    database.logHistory ("phpunit", 2014, 1, 10, 15, 50);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);

    // Log values for February 2014, and don't expect them when requesting the history for January ...
    database.logHistory ("phpunit", 2014, 2, 10, 15, 51);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);

    // ... but get those values when requesting history for February.
    history = database.getHistory ("phpunit", 2014, 2);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 51, history[0].complete);

    // Log another history entry for January 2014, and expect two correct entries for this month.
    database.logHistory ("phpunit", 2014, 1, 11, 16, 55);
    history = database.getHistory ("phpunit", 2014, 1);
    evaluate (__LINE__, __func__, 10, history[0].day);
    evaluate (__LINE__, __func__, 15, history[0].tasks);
    evaluate (__LINE__, __func__, 50, history[0].complete);
    evaluate (__LINE__, __func__, 11, history[1].day);
    evaluate (__LINE__, __func__, 16, history[1].tasks);
    evaluate (__LINE__, __func__, 55, history[1].complete);
  }
}


void test_database_mail ()
{
  // Optimize / Trim.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    database_mail.optimize ();
    database_mail.trim ();
  }
  // Empty.
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    int count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 0, count);
    
    vector <Database_Mail_User> mails = database_mail.getMails ();
    evaluate (__LINE__, __func__, 0, mails.size());
    
    vector <int> mails_to_send = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, {}, mails_to_send);
  }
  // Normal Cycle
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    database_mail.send ("phpunit", "subject", "body");

    int count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 1, count);

    vector <Database_Mail_User> mails = database_mail.getMails ();
    evaluate (__LINE__, __func__, "subject", mails [0].subject);
    
    Database_Mail_Item mail = database_mail.get (1);
    evaluate (__LINE__, __func__, "phpunit", mail.username);
    evaluate (__LINE__, __func__, "body", mail.body);

    database_mail.erase (1);

    count = database_mail.getMailCount ();
    evaluate (__LINE__, __func__, 0, count);
  }
  // Normal Postpone
  {
    refresh_sandbox (true);
    Database_Users database_users = Database_Users ();
    database_users.create ();
    Webserver_Request request = Webserver_Request ();
    Database_Mail database_mail = Database_Mail (&request);
    database_mail.create ();
    request.session_logic ()->setUsername ("phpunit");

    database_mail.send ("phpunit", "subject", "body");

    vector <int> mails = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, 1, mails.size ());
    
    database_mail.postpone (1);
    mails = database_mail.getMailsToSend ();
    evaluate (__LINE__, __func__, 0, mails.size ());
  }
}


void test_database_navigation ()
{
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    database.trim ();
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();

    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();

    // Record one entry. As a result there should be no previous entry.
    database.record (time, "phpunit", 1, 2, 3);
    bool previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry, with the same time.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 4 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    time += 4;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 5 seconds later.
    // This should remove the already existing entry.
    // As a result there should be no previous entry.
    time += 5;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, false, previous);

    // Record another entry 6 seconds later.
    // This should not remove the already existing entry.
    // As a result there should be a previous entry.
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    previous = database.previousExists ("phpunit");
    evaluate (__LINE__, __func__, true, previous);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();
    // Record one entry, and another 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Get previous entry, which should be the first one entered.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();
    // Record one entry, and another 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Get previous entry for another user: It should not be there.
    Passage passage = database.getPrevious ("phpunit2");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();
    // Record three entries, each one 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    time += 6;
    database.record (time, "phpunit", 7, 8, 9);
    // Get previous entry, which should be the second one entered.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();
    // Record five entries, each one 6 seconds later.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    time += 6;
    database.record (time, "phpunit", 7, 8, 9);
    time += 6;
    database.record (time, "phpunit", 10, 11, 12);
    time += 6;
    database.record (time, "phpunit", 13, 14, 15);
    // Get previous entry, which should be the last but one passage recorded.
    Passage passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 10, passage.book);
    evaluate (__LINE__, __func__, 11, passage.chapter);
    evaluate (__LINE__, __func__, "12", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // There should be no next passage.
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Use current time.
    int time = filter_string_date_seconds_since_epoch ();
    // Record several entries, all spaced apart by 6 seconds.
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 1, 2, 3);
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  {
    refresh_sandbox (true);
    Database_Navigation database = Database_Navigation ();
    database.create ();
    // Record two entries at an interval.
    int time = filter_string_date_seconds_since_epoch ();
    database.record (time, "phpunit", 1, 2, 3);
    time += 6;
    database.record (time, "phpunit", 4, 5, 6);
    // Next entry is not there.
    Passage passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
    // Previous entry should be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    // Next entry should be there since we moved to the previous one.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
    // Previous entry should be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    // Previous entry before previous entry should not be there.
    passage = database.getPrevious ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
    // Next entry should be there since we moved to the previous one.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 4, passage.book);
    evaluate (__LINE__, __func__, 5, passage.chapter);
    evaluate (__LINE__, __func__, "6", passage.verse);
    // The entry next to the next entry should not be there.
    passage = database.getNext ("phpunit");
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
}

/* Todo

*/
