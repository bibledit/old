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


#include <database/modifications.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/date.h>
#include <database/sqlite.h>


// Database resilience: All information is stored on the filesystem.
// The database is only used as an index.
// It is re-indexed every night.


Database_Modifications::Database_Modifications ()
{
}


Database_Modifications::~Database_Modifications ()
{
}


sqlite3 * Database_Modifications::connect ()
{
  return database_sqlite_connect ("modifications");
}


// Delete the entire database
void Database_Modifications::erase ()
{
  string file = filter_url_create_root_path ("databases", "modifications.sqlite");
  filter_url_unlink (file);
}


void Database_Modifications::create ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "DROP TABLE IF EXISTS notifications");
  string sql;
  sql = 
    "CREATE TABLE IF NOT EXISTS notifications ("
    " identifier integer,"
    " timestamp integer,"
    " username text,"
    " category text,"
    " bible text,"
    " book integer,"
    " chapter integer,"
    " verse integer,"
    " modification text"
    ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Code dealing with the "teams" table.


string Database_Modifications::teamFolder ()
{
  return filter_url_create_root_path ("databases", "modifications", "team");
}


string Database_Modifications::teamFile (const string& bible, int book, int chapter)
{
  return filter_url_create_path (teamFolder (), bible + "." + convert_to_string (book) + "." + convert_to_string (chapter));
}


// Returns true if diff data exists for the chapter.
// Else it returns false.
bool Database_Modifications::teamDiffExists (const string& bible, int book, int chapter)
{
  string file = teamFile (bible, book, chapter);
  return file_exists (file);
}


// Stores diff data for a "bible" (string) and book (int) and chapter (int).
void Database_Modifications::storeTeamDiff (const string& bible, int book, int chapter)
{
  // Return when the diff exists.
  if (teamDiffExists (bible, book, chapter)) return;

  // Retrieve current chapter USFM data.
  Database_Bibles database_bibles = Database_Bibles ();
  string data = database_bibles.getChapter (bible, book, chapter);

  // Store.
  string file = teamFile (bible, book, chapter);
  filter_url_file_put_contents (file, data);
}


// Gets the diff data as a string.
string Database_Modifications::getTeamDiff (const string& bible, int book, int chapter)
{
  string file = teamFile (bible, book, chapter);
  return filter_url_file_get_contents (file);
}


// Stores diff data for all chapters in a "bible" (string) and book (int).
void Database_Modifications::storeTeamDiffBook (const string& bible, int book)
{
  Database_Bibles database_bibles = Database_Bibles ();
  vector <int> chapters = database_bibles.getChapters (bible, book);
  for (auto & chapter : chapters) {
    storeTeamDiff (bible, book, chapter);
  }
}


// Stores diff data for all books in a "bible" (string).
void Database_Modifications::storeTeamDiffBible (const string& bible)
{
  Database_Bibles database_bibles = Database_Bibles ();
  vector <int> books = database_bibles.getBooks (bible);
  for (auto & book : books) {
    storeTeamDiffBook (bible, book);
  }
}


// Deletes the diffs for a whole Bible.
void Database_Modifications::deleteTeamDiffBible (const string& bible)
{
  string pattern = bible + ".";
  int length = pattern.length ();
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto & file : files) {
    if (file.substr (0, length) != pattern) continue;
    filter_url_unlink (filter_url_create_path (teamFolder (), file));
  }
}


// Deletes the diffs for one chapter of a Bible.
void Database_Modifications::deleteTeamDiffChapter (const string& bible, int book, int chapter)
{
  string file = teamFile (bible, book, chapter);
  filter_url_unlink (file);
}


// Returns an array with the available chapters that have diff data in a book in a Bible.
vector <int> Database_Modifications::getTeamDiffChapters (const string& bible, int book)
{
  string pattern = bible + "." + convert_to_string (book) + ".";
  int length = pattern.length ();
  vector <int> chapters;
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto & file : files) {
    if (file.substr (0, length) != pattern) continue;
    vector <string> bits = filter_string_explode (file, '.');
    if (bits.size() != 3) continue;
    chapters.push_back (convert_to_int (bits [2]));
  }
  sort (chapters.begin(), chapters.end());
  return chapters;
}


// Returns the number of items in bible that have diff data.
// The bible can be a name or an identifier. This is because the bible identifier may no longer exist.
int Database_Modifications::getTeamDiffCount (const string& bible)
{
  string pattern = bible + ".";
  int length = pattern.length ();
  int count = 0;
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto & file : files) {
    if (file.substr (0, length) != pattern) continue;
    count++;
  }
  return count;
}


// Returns an array with the available books that have diff data in a Bible.
// The bible can be a name, or an identifier. This is because the bible identifier may no longer exist.
vector <int> Database_Modifications::getTeamDiffBooks (const string& bible)
{
  vector <int> books;
  string pattern = bible + ".";
  int length = pattern.length ();
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto & file : files) {
    if (file.substr (0, length) != pattern) continue;
    vector <string> bits = filter_string_explode (file, '.');
    if (bits.size() != 3) continue;
    books.push_back (convert_to_int (bits [1]));
  }
  set <int> bookset (books.begin (), books.end());
  books.assign (bookset.begin(), bookset.end());
  sort (books.begin(), books.end());
  return books;
}


// Returns an array with the available Bibles that have diff data.
vector <string> Database_Modifications::getTeamDiffBibles ()
{
  vector <string> bibles;
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto & file : files) {
    vector <string> bits = filter_string_explode (file, '.');
    if (bits.size() != 3) continue;
    bibles.push_back (bits [0]);
  }
  set <string> bibleset (bibles.begin (), bibles.end());
  bibles.assign (bibleset.begin(), bibleset.end());
  sort (bibles.begin(), bibles.end());
  return bibles;
}


// Truncates all team modifications.
void Database_Modifications::truncateTeams ()
{
  vector <string> files = filter_url_scandir (teamFolder ());
  for (auto file : files) {
    filter_url_unlink (filter_url_create_path (teamFolder (), file));
  }
}


// Code dealing with the "users" table.


string Database_Modifications::userMainFolder ()
{
  return filter_url_create_root_path ("databases", "modifications", "users");
}


string Database_Modifications::userUserFolder (const string& username)
{
  return filter_url_create_path (userMainFolder (), username);
}


string Database_Modifications::userBibleFolder (const string& username, const string& bible)
{
  return filter_url_create_path (userUserFolder (username), bible);
}


string Database_Modifications::userBookFolder (const string& username, const string& bible, int book)
{
  return filter_url_create_path (userBibleFolder (username, bible), convert_to_string (book));
}


string Database_Modifications::userChapterFolder (const string& username, const string& bible, int book, int chapter)
{
  return filter_url_create_path (userBookFolder (username, bible, book), convert_to_string (chapter));
}


string Database_Modifications::userNewIDFolder (const string& username, const string& bible, int book, int chapter, int newID)
{
  return filter_url_create_path (userChapterFolder (username, bible, book, chapter), convert_to_string (newID));
}


string Database_Modifications::userTimeFile (const string& username, const string& bible, int book, int chapter, int newID)
{
  return filter_url_create_path (userNewIDFolder (username, bible, book, chapter, newID), "time");
}


string Database_Modifications::userOldIDFile (const string& username, const string& bible, int book, int chapter, int newID)
{
  return filter_url_create_path (userNewIDFolder (username, bible, book, chapter, newID), "oldid");
}


string Database_Modifications::userOldTextFile (const string& username, const string& bible, int book, int chapter, int newID)
{
  return filter_url_create_path (userNewIDFolder (username, bible, book, chapter, newID), "oldtext");
}


string Database_Modifications::userNewTextFile (const string& username, const string& bible, int book, int chapter, int newID)
{
  return filter_url_create_path (userNewIDFolder (username, bible, book, chapter, newID), "newtext");
}


void Database_Modifications::recordUserSave (const string& username, const string& bible, int book, int chapter, int oldID, const string& oldText, int newID, const string& newText)
{
  // This entry is saved in a deep folder structure with the new ID in it.
  string folder = userNewIDFolder (username, bible, book, chapter, newID);
  if (!file_exists (folder)) filter_url_mkdir (folder);
  // The other data is stored in separate files in the newID folder.
  string timeFile = userTimeFile (username, bible, book, chapter, newID);
  filter_url_file_put_contents (timeFile, convert_to_string (filter_date_seconds_since_epoch ()));
  string oldIDFile = userOldIDFile (username, bible, book, chapter, newID);
  filter_url_file_put_contents (oldIDFile, convert_to_string (oldID));
  string oldTextFile = userOldTextFile (username, bible, book, chapter, newID);
  filter_url_file_put_contents (oldTextFile, oldText);
  string newTextFile = userNewTextFile (username, bible, book, chapter, newID);
  filter_url_file_put_contents (newTextFile, newText);
}


void Database_Modifications::clearUserUser (const string& username)
{
  string folder = userUserFolder (username);
  filter_url_rmdir (folder);
}


vector <string> Database_Modifications::getUserUsernames ()
{
  string folder = userMainFolder ();
  vector <string> usernames = filter_url_scandir (folder);
  return usernames;
}


vector <string> Database_Modifications::getUserBibles (const string& username)
{
  string folder = userUserFolder (username);
  return filter_url_scandir (folder);
}


vector <int> Database_Modifications::getUserBooks (const string& username, const string& bible)
{
  string folder = userBibleFolder (username, bible);
  vector <string> files = filter_url_scandir (folder);
  vector <int> books;
  for (auto & file : files) {
    books.push_back (convert_to_int (file));
  }
  sort (books.begin(), books.end());
  return books;
}


vector <int> Database_Modifications::getUserChapters (const string& username, const string& bible, int book)
{
  string folder = userBookFolder (username, bible, book);
  vector <string> files = filter_url_scandir (folder);
  vector <int> chapters;
  for (auto & file : files) {
    chapters.push_back (convert_to_int (file));
  }
  sort (chapters.begin(), chapters.end());
  return chapters;
}


vector <Database_Modifications_Id> Database_Modifications::getUserIdentifiers (const string& username, const string& bible, int book, int chapter)
{
  string folder = userChapterFolder (username, bible, book, chapter);
  vector <Database_Modifications_Id> ids;
  vector <string> newids = filter_url_scandir (folder);
  for (auto & newid : newids) {
    string file = userOldIDFile (username, bible, book, chapter, convert_to_int (newid));
    string oldid = filter_url_file_get_contents (file);
    Database_Modifications_Id id;
    id.oldid = convert_to_int (oldid);
    id.newid = convert_to_int (newid);
    ids.push_back (id);
  }
  return ids;
}


Database_Modifications_Text Database_Modifications::getUserChapter (const string& username, const string& bible, int book, int chapter, int newID)
{
  string oldfile = userOldTextFile (username, bible, book, chapter, newID);
  string newfile = userNewTextFile (username, bible, book, chapter, newID);
  string oldtext = filter_url_file_get_contents (oldfile);
  string newtext = filter_url_file_get_contents (newfile);
  Database_Modifications_Text data;
  data.oldtext = oldtext;
  data.newtext = newtext;
  return data;
}


int Database_Modifications::getUserTimestamp (const string& username, const string& bible, int book, int chapter, int newID)
{
  string file = userTimeFile (username, bible, book, chapter, newID);
  string contents = filter_url_file_get_contents (file);
  int time = convert_to_int (contents);
  if (time > 0) return time;
  return filter_date_seconds_since_epoch ();
}


// Code dealing with the notifications.


string Database_Modifications::notificationsMainFolder ()
{
  return filter_url_create_root_path ("databases", "modifications", "notifications");
}


string Database_Modifications::notificationIdentifierFolder (int identifier)
{
  return filter_url_create_path (notificationsMainFolder (), convert_to_string (identifier));
}


string Database_Modifications::notificationTimeFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "time");
}


string Database_Modifications::notificationUserFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "user");
}


string Database_Modifications::notificationCategoryFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "category");
}


string Database_Modifications::notificationBibleFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "bible");
}


string Database_Modifications::notificationPassageFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "passage");
}


string Database_Modifications::notificationOldtextFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "oldtext");
}


string Database_Modifications::notificationModificationFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "modification");
}


string Database_Modifications::notificationNewtextFile (int identifier)
{
  return filter_url_create_path (notificationIdentifierFolder (identifier), "newtext");
}


int Database_Modifications::getNextAvailableNotificationIdentifier ()
{
  // Read the existing identifiers in the folder.
  vector <string> files = filter_url_scandir (notificationsMainFolder ());
  // Sort from low to high.
  vector <int> identifiers;
  for (auto & file : files) identifiers.push_back (convert_to_int (file));
  sort (identifiers.begin(), identifiers.end());
  // Fetch the last and highest identifier.
  int identifier = 0;
  size_t length = identifiers.size();
  if (length) identifier = identifiers [length - 1];
  // Take next one.
  identifier++;
  // Done.
  return identifier;
}


void Database_Modifications::recordNotification (const vector <string> & users, const string& category, const string& bible, int book, int chapter, int verse, const string& oldtext, const string& modification, const string& newtext)
{
  // Normally this function is called just after midnight.
  // It would then put the current time on changes made the day before.
  // Make a correction for that by subtracting 6 hours.
  int timestamp = filter_date_seconds_since_epoch () - 21600;
  for (auto & user : users) {
    int identifier = getNextAvailableNotificationIdentifier ();
    filter_url_mkdir (notificationIdentifierFolder (identifier));
    string file;
    file = notificationTimeFile (identifier);
    filter_url_file_put_contents (file, convert_to_string (timestamp));
    file = notificationUserFile (identifier);
    filter_url_file_put_contents (file, user);
    file = notificationCategoryFile (identifier);
    filter_url_file_put_contents (file, category);
    file = notificationBibleFile (identifier);
    filter_url_file_put_contents (file, bible);
    string passage = convert_to_string (book) + "." + convert_to_string (chapter) + "." + convert_to_string (verse);
    file = notificationPassageFile (identifier);
    filter_url_file_put_contents (file, passage);
    file = notificationOldtextFile (identifier);
    filter_url_file_put_contents (file, oldtext);
    file = notificationModificationFile (identifier);
    filter_url_file_put_contents (file, modification);
    file = notificationNewtextFile (identifier);
    filter_url_file_put_contents (file, newtext);
  }
}


void Database_Modifications::indexTrimAllNotifications ()
{
  // Delete the index database and create an empty one.
  erase ();
  create ();

  // Change notifications expire after 30 days.
  int expiry_time = filter_date_seconds_since_epoch () - 2592000;

  // Database: Connect and speed it up.
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "PRAGMA synchronous = OFF;");

  // Go through the notifications on disk.
  vector <string> sidentifiers = filter_url_scandir (notificationsMainFolder ());
  vector <int> identifiers;
  for (auto & s : sidentifiers) identifiers.push_back (convert_to_int (s));
  sort (identifiers.begin(), identifiers.end());
  for (auto & identifier : identifiers) {

    // Fetch the data from the filesystem and validate it.
    bool valid = true;

    int timestamp = convert_to_int (filter_url_file_get_contents (notificationTimeFile (identifier)));
    if (timestamp < expiry_time) valid = false;

    string user = filter_url_file_get_contents (notificationUserFile (identifier));
    if (user == "") valid = false;

    string category = filter_url_file_get_contents (notificationCategoryFile (identifier));

    string bible = filter_url_file_get_contents (notificationBibleFile (identifier));
    if (bible == "") valid = false;

    string spassage = filter_url_file_get_contents (notificationPassageFile (identifier));
    vector <string> vpassage = filter_string_explode (spassage, '.');
    if (vpassage.size () != 3) valid = false;
    
    int book = 0;
    if (valid) book = convert_to_int (vpassage [0]);
    if (book == 0) valid = false;

    int chapter = 0;
    if (valid) chapter = convert_to_int (vpassage [1]);
    if (vpassage [1].empty()) valid = false;
    
    int verse = 0;
    if (valid) verse = convert_to_int (vpassage [2]);
    if (vpassage [2].empty()) valid = false;

    string modification = getNotificationModification (identifier);
    if (modification.empty()) valid = false;
   
    if (valid) {
      // Store valid data in the database.
      SqliteSQL sql = SqliteSQL ();
      sql.add ("INSERT INTO notifications VALUES (");
      sql.add (identifier);
      sql.add (",");
      sql.add (timestamp);
      sql.add (",");
      sql.add (user);
      sql.add (",");
      sql.add (category);
      sql.add (",");
      sql.add (bible);
      sql.add (",");
      sql.add (book);
      sql.add (",");
      sql.add (chapter);
      sql.add (",");
      sql.add (verse);
      sql.add (",");
      sql.add (modification);
      sql.add (");");
      database_sqlite_exec (db, sql.sql);
    } else {
      // Delete invalid / expired data.
      deleteNotification (identifier, db);
    }
  }
  
  database_sqlite_disconnect (db);
}


vector <int> Database_Modifications::getNotificationIdentifiers (const string& username, bool limit) // Todo
{
  vector <int> ids;

  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT identifier FROM notifications");
  if (username != "") {
    sql.add ("WHERE username =");
    sql.add (username);
  }
  // Sort on reference, so that related change notifications are near each other.
  sql.add ("ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC");
  // Optionally limit the hits.
  if (limit) sql.add ("LIMIT 1000");
  sql.add (";");

  sqlite3 * db = connect ();
  vector <string> sidentifiers = database_sqlite_query (db, sql.sql) ["identifier"];
  database_sqlite_disconnect (db);
  for (auto & identifier : sidentifiers) {
    ids.push_back (convert_to_int (identifier));
  }

  return ids;
}


// This gets the identifiers of the personal change proposals.
// For easier comparison, it also gets the identifiers of the changes
// in the verses that have personal change proposals.
vector <int> Database_Modifications::getNotificationPersonalIdentifiers (const string& username, const string& category, bool limit)
{
  sqlite3 * db = connect ();

  // Get all personal change proposals.
  vector <int> personalIDs;
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT identifier FROM notifications WHERE username =");
  sql.add (username);
  sql.add ("AND category =");
  sql.add (category);
  sql.add ("ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC");
  // Optionally limit the hits.
  if (limit) sql.add ("LIMIT 1000");
  sql.add (";");

  vector <string> sidentifiers = database_sqlite_query (db, sql.sql) ["identifier"];
  for (auto & identifier : sidentifiers) {
    personalIDs.push_back (convert_to_int (identifier));
  }

  vector <int> allIDs;

  // Go through each of the personal change proposals.
  for (int & personalID : personalIDs) {
    // Add the personal change proposal to the results.
    allIDs.push_back (personalID);
    // Get the Bible and passage for this change proposal.
    string bible = getNotificationBible (personalID);
    Passage passage = getNotificationPassage (personalID);
    int book = passage.book;
    int chapter = passage.chapter;
    int verse = convert_to_int (passage.verse);
    // Look for change proposals for this Bible and passage.
    SqliteSQL sql = SqliteSQL ();
    sql.add ("SELECT identifier FROM notifications WHERE username =");
    sql.add (username);
    sql.add ("AND bible =");
    sql.add (bible);
    sql.add ("AND book =");
    sql.add (book);
    sql.add ("AND chapter =");
    sql.add (chapter);
    sql.add ("AND verse =");
    sql.add (verse);
    sql.add ("ORDER BY identifier ASC;");
    vector <string> sidentifiers = database_sqlite_query (db, sql.sql) ["identifier"];
    for (auto & identifier : sidentifiers) {
      int id = convert_to_int (identifier);
      // Add the identifier if it's not yet in.
      if (find (allIDs.begin(), allIDs.end(), id) == allIDs.end()) {
        allIDs.push_back (id);
      }
    }
  }
  
  database_sqlite_disconnect (db);

  return allIDs;
}


// This gets the identifiers of the team's changes.
vector <int> Database_Modifications::getNotificationTeamIdentifiers (const string& username, const string& category, bool limit)
{
  vector <int> ids;
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT identifier FROM notifications WHERE username =");
  sql.add (username);
  sql.add ("AND category =");
  sql.add (category);
  sql.add ("ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC");
  if (limit) sql.add ("LIMIT 1000");
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> sidentifiers = database_sqlite_query (db, sql.sql) ["identifier"];
  database_sqlite_disconnect (db);
  for (auto & sid : sidentifiers) {
    ids.push_back (convert_to_int (sid));
  }
  return ids;
}


void Database_Modifications::deleteNotification (int identifier, sqlite3 * db)
{
  // Delete from the filesystem.
  string folder = notificationIdentifierFolder (identifier);
  filter_url_rmdir (folder);
  // Delete from the database.
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM notifications WHERE identifier =");
  sql.add (identifier);
  sql.add (";");
  // Make a very short connection to the database, 
  // to prevent corruption when a user deletes lots of changes notifications 
  // by keeping the delete key pressed.
  bool local_connection = (db == NULL);
  if (local_connection) db = connect ();
  database_sqlite_exec (db, sql.sql);
  if (local_connection) database_sqlite_disconnect (db);
}


int Database_Modifications::getNotificationTimeStamp (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT timestamp FROM notifications WHERE identifier =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> timestamps = database_sqlite_query (db, sql.sql) ["timestamp"];
  database_sqlite_disconnect (db);
  int time = filter_date_seconds_since_epoch ();
  for (auto & stamp : timestamps) {
    time = convert_to_int (stamp);
  }
  return time;
}


string Database_Modifications::getNotificationCategory (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT category FROM notifications WHERE identifier =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> categories = database_sqlite_query (db, sql.sql) ["category"];
  database_sqlite_disconnect (db);
  string category = "";
  for (auto & row : categories) {
    category = row;
  }
  return category;
}


string Database_Modifications::getNotificationBible (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT bible FROM notifications WHERE identifier =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> bibles = database_sqlite_query (db, sql.sql) ["bible"];
  database_sqlite_disconnect (db);
  string bible = "";
  for (auto & item : bibles) {
    bible = item;
  }
  return bible;
}


Passage Database_Modifications::getNotificationPassage (int id)
{
  Passage passage;
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT book, chapter, verse FROM notifications WHERE identifier =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  for (unsigned int i = 0; i < books.size (); i++) {
    passage.book = convert_to_int (books [i]);
    passage.chapter = convert_to_int (chapters [i]);
    passage.verse = verses [i];
  }
  return passage;
}


string Database_Modifications::getNotificationOldText (int id)
{
  string file = notificationOldtextFile (id);
  return filter_url_file_get_contents (file);
}


string Database_Modifications::getNotificationModification (int id)
{
  string file = notificationModificationFile (id);
  return filter_url_file_get_contents (file);
}


string Database_Modifications::getNotificationNewText (int id)
{
  string file = notificationNewtextFile (id);
  return filter_url_file_get_contents (file);
}


void Database_Modifications::clearNotificationsUser (const string& username)
{
  vector <int> identifiers = getNotificationIdentifiers (username);
  sqlite3 * db = connect ();
  // Transaction speeds up the operation.
  database_sqlite_exec (db, "BEGIN;");
  for (auto& identifier : identifiers) {
    deleteNotification (identifier, db);
  }
  database_sqlite_exec (db, "COMMIT;");
  database_sqlite_disconnect (db);
}


// This function deletes personal change proposals and their matching change notifications.
void Database_Modifications::clearNotificationMatches (const string& username, const string& personal, const string& team)
{
  sqlite3 * db = connect ();
  
  // Select all identifiers of the personal change proposals.
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT identifier FROM notifications WHERE username =");
  sql.add (username);
  sql.add ("AND category =");
  sql.add (personal);
  sql.add (";");

  vector <int> personals;
  vector <string> result = database_sqlite_query (db, sql.sql) ["identifier"];
  for (auto & item : result) {
    personals.push_back (convert_to_int (item));
  }
  
  // Matches to be deleted.
  vector <int> deletes;

  // Go through each of the personal change proposals.
  for (auto & personalID : personals) {
    string bible = getNotificationBible (personalID);
    Passage passage = getNotificationPassage (personalID);
    int book = passage.book;
    int chapter = passage.chapter;
    int verse = convert_to_int (passage.verse);
    string modification = getNotificationModification (personalID);
    // Get all matching identifiers from the team's change notifications.
    SqliteSQL sql = SqliteSQL ();
    sql.add ("SELECT identifier FROM notifications WHERE username =");
    sql.add (username);
    sql.add ("AND category =");
    sql.add (team);
    sql.add ("AND bible =");
    sql.add (bible);
    sql.add ("AND book =");
    sql.add (book);
    sql.add ("AND chapter =");
    sql.add (chapter);
    sql.add ("AND verse =");
    sql.add (verse);
    sql.add ("AND modification =");
    sql.add (modification);
    sql.add (";");
    vector <int> teamMatches;
    vector <string> result = database_sqlite_query (db, sql.sql) ["identifier"];
    for (auto & item : result) {
      teamMatches.push_back (convert_to_int (item));
    }
    // There should be exactly one candidate for the matches to be removed.
    // If there are none, it means that the personal change didn't make it to the team's text.
    // If there are two or more matching changes, then one could have undone the other, so should not be automatically removed.
    if (teamMatches.size () == 1) {
      // Check there are only two change notifications for this user / Bible / book / chapter / verse.
      // If there are more, we can't be sure that the personal change proposal was not overwritten somehow.
      vector <int> passageMatches;
      SqliteSQL sql = SqliteSQL ();
      sql.add ("SELECT identifier FROM notifications WHERE username =");
      sql.add (username);
      sql.add ("AND bible =");
      sql.add (bible);
      sql.add ("AND book =");
      sql.add (book);
      sql.add ("AND chapter =");
      sql.add (chapter);
      sql.add ("AND verse =");
      sql.add (verse);
      sql.add (";");
      vector <string> result = database_sqlite_query (db, sql.sql) ["identifier"];
      for (auto & item : result) {
        passageMatches.push_back (convert_to_int (item));
      }
      if (passageMatches.size () == 2) {
        // Store the personal change proposal to be deleted.
        // Store the matching change notification to be deleted also.
        for (auto & passageMatch : passageMatches) {
          deletes.push_back (passageMatch);
        }
      }
    }
  }

  // Delete all stored identifiers to be deleted.
  for (auto & id : deletes) {
    deleteNotification (id, db);
  }
  
  database_sqlite_disconnect (db);
}


