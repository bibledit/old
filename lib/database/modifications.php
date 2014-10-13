<?php
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


// Database resilience: All information is stored on the filesystem.
// The database is only used as an index.
// It is re-indexed every night.


class Database_Modifications
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Modifications ();
    }
    return self::$instance;
  }


  // Delete the entire database
  static public function delete ()
  {
    $file = realpath (dirname (__DIR__) . "/databases/modifications.sqlite");
    @unlink ($file);
  }
  
  
  private function connect ()
  {
    $db = Database_SQLite::connect ("modifications");
    return $db;
  }


  public function create ()
  {
    $db = $this->connect ();
    Database_SQLite::exec ($db, "DROP TABLE IF EXISTS notifications");
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS notifications (
  identifier integer,
  timestamp integer,
  username text,
  category text,
  bible text,
  book integer,
  chapter integer,
  verse integer,
  modification text
);
EOD;
    Database_SQLite::exec ($db, $sql);
    unset ($db);
  }


  // Code dealing with the "teams" table.
  

  private function teamFolder ()
  {
    $folder = realpath (__DIR__ . "/../databases/modifications/team");
    return $folder;
  }
  
  
  private function teamFile ($bible, $book, $chapter)
  {
    $file = $this->teamFolder () . "/$bible.$book.$chapter";
    return $file;
  }
  
  
  // Returns true if diff data exists for the chapter.
  // Else it returns false.
  public function teamDiffExists ($bible, $book, $chapter)
  {
    $file = $this->teamFile ($bible, $book, $chapter);
    return file_exists ($file);
  }


  // Stores diff data for a "bible" (string) and $book (int) and $chapter (int).
  public function storeTeamDiff ($bible, $book, $chapter)
  {
    // Return when the diff exists.
    if ($this->teamDiffExists ($bible, $book, $chapter)) {
      return;
    }

    // Retrieve current chapter USFM data.
    $database_bibles = Database_Bibles::getInstance ();
    $data = $database_bibles->getChapter ($bible, $book, $chapter);

    // Store.
    $file = $this->teamFile ($bible, $book, $chapter);
    file_put_contents ($file, $data);
  }


  // Gets the diff data as a string.
  public function getTeamDiff ($bible, $book, $chapter)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $file = $this->teamFile ($bible, $book, $chapter);
    @$usfm = file_get_contents ($file);
    if ($usfm) return $usfm;
    return "";
  }


  // Stores diff data for all chapters in a "bible" (string) and $book (int).
  public function storeTeamDiffBook ($bible, $book)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $this->storeTeamDiff ($bible, $book, $chapter);
    }
  }


  // Stores diff data for all books in a "bible" (string).
  public function storeTeamDiffBible ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $this->storeTeamDiffBook ($bible, $book);
    }
  }


  // Deletes the diffs for a whole Bible.
  public function deleteTeamDiffBible ($bible)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $files = glob ($this->teamFolder () . "/$bible.*");
    foreach ($files as $file) {
      unlink ($file);
    }
  }


  // Deletes the diffs for one chapter of a Bible.
  public function deleteTeamDiffChapter ($bible, $book, $chapter)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $file = $this->teamFile ($bible, $book, $chapter);
    unlink ($file);
  }


  // Returns an array with the available chapters that have diff data in a $book in a Bible.
  public function getTeamDiffChapters ($bible, $book)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $chapters = array ();
    $files = glob ($this->teamFolder () . "/$bible.$book.*");
    foreach ($files as $file) {
      $file = basename ($file);
      $file = explode (".", $file);
      $chapters [] = $file [2];
    }
    sort ($chapters, SORT_NUMERIC);
    return $chapters;
  }


  // Returns the number of items in $bible that have diff data.
  // The $bible can be a name or an identifier. This is because the $bible identifier may no longer exist.
  public function getTeamDiffCount ($bible)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $files = glob ($this->teamFolder () . "/$bible.*");
    return count ($files);
  }


  // Returns an array with the available books that have diff data in a Bible.
  // The $bible can be a name, or an identifier. This is because the $bible identifier may no longer exist.
  public function getTeamDiffBooks ($bible)
  {
    if (is_numeric ($bible)) {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getName ($bible);
    }
    $books = array ();
    $files = glob ($this->teamFolder () . "/$bible.*");
    foreach ($files as $file) {
      $file = basename ($file);
      $file = explode (".", $file);
      $books [] = $file [1];
    }
    $books = array_unique ($books);
    sort ($books, SORT_NUMERIC);
    return $books;
  }


  // Returns an array with the available Bibles that have diff data.
  public function getTeamDiffBibles ()
  {
    $bibles = array ();
    $files = glob ($this->teamFolder () . "/*");
    foreach ($files as $file) {
      $file = basename ($file);
      $file = explode (".", $file);
      $bibles [] = $file [0];
    }
    $bibles = array_unique ($bibles);
    sort ($bibles);
    return $bibles;
  }


  // Truncates all team modifications.
  public function truncateTeams ()
  {
    $files = glob ($this->teamFolder () . "/*");
    foreach ($files as $file) {
      unlink ($file);
    }
  }


  // Code dealing with the "users" table.


  private function userMainFolder ()
  {
    return realpath (__DIR__ . "/../databases/modifications/users");
  }


  private function userUserFolder ($username)
  {
    return $this->userMainFolder () . "/$username";
  }
  
  
  private function userBibleFolder ($username, $bible)
  {
    return $this->userUserFolder ($username) . "/$bible";
  }
  
  
  private function userBookFolder ($username, $bible, $book)
  {
    return $this->userBibleFolder ($username, $bible) . "/$book";
  }
  
  
  private function userChapterFolder ($username, $bible, $book, $chapter)
  {
    return $this->userBookFolder ($username, $bible, $book) . "/$chapter";
  }
  
  
  private function userNewIDFolder ($username, $bible, $book, $chapter, $newID)
  {
    return $this->userChapterFolder ($username, $bible, $book, $chapter) . "/$newID";
  }


  private function userTimeFile ($username, $bible, $book, $chapter, $newID)
  {
    return $this->userNewIDFolder ($username, $bible, $book, $chapter, $newID) . "/time";
  }


  private function userOldIDFile ($username, $bible, $book, $chapter, $newID)
  {
    return $this->userNewIDFolder ($username, $bible, $book, $chapter, $newID) . "/oldid";
  }


  private function userOldTextFile ($username, $bible, $book, $chapter, $newID)
  {
    return $this->userNewIDFolder ($username, $bible, $book, $chapter, $newID) . "/oldtext";
  }


  private function userNewTextFile ($username, $bible, $book, $chapter, $newID)
  {
    return $this->userNewIDFolder ($username, $bible, $book, $chapter, $newID) . "/newtext";
  }
  
  
  public function recordUserSave ($username, $bible, $book, $chapter, $oldID, $oldText, $newID, $newText)
  {
    // This entry is saved in a deep folder structure with the new ID in it.
    $folder = $this->userNewIDFolder ($username, $bible, $book, $chapter, $newID);
    if (!file_exists ($folder)) mkdir ($folder, 0777, true);
    // The other data is stored in separate files in the newID folder.
    $timeFile = $this->userTimeFile ($username, $bible, $book, $chapter, $newID);
    file_put_contents ($timeFile, time ());
    $oldIDFile = $this->userOldIDFile ($username, $bible, $book, $chapter, $newID);
    file_put_contents ($oldIDFile, $oldID);
    $oldTextFile = $this->userOldTextFile ($username, $bible, $book, $chapter, $newID);
    file_put_contents ($oldTextFile, $oldText);
    $newTextFile = $this->userNewTextFile ($username, $bible, $book, $chapter, $newID);
    file_put_contents ($newTextFile, $newText);
  }


  public function debug ()
  {
  }


  public function clearUserUser ($username)
  {
    $folder = $this->userUserFolder ($username);
    Filter_Rmdir::rmdir ($folder);
  }


  public function getUserUsernames ()
  {
    $folder = $this->userMainFolder ();
    $usernames = scandir ($folder);
    $usernames = Filter_Folders::cleanup ($usernames);
    return $usernames;
  }


  public function getUserBibles ($username)
  {
    $folder = $this->userUserFolder ($username);
    @$bibles = scandir ($folder);
    if (!$bibles) return array ();
    $bibles = Filter_Folders::cleanup ($bibles);
    return $bibles;
  }


  public function getUserBooks ($username, $bible)
  {
    $folder = $this->userBibleFolder ($username, $bible);
    @$books = scandir ($folder);
    if (!$books) return array ();
    $books = Filter_Folders::cleanup ($books);
    sort ($books, SORT_NUMERIC);
    return $books;
  }


  public function getUserChapters ($username, $bible, $book)
  {
    $folder = $this->userBookFolder ($username, $bible, $book);
    @$chapters = scandir ($folder);
    if (!$chapters) return array ();
    $chapters = Filter_Folders::cleanup ($chapters);
    sort ($chapters, SORT_NUMERIC);
    return $chapters;
  }


  public function getUserIdentifiers ($username, $bible, $book, $chapter)
  {
    $folder = $this->userChapterFolder ($username, $bible, $book, $chapter);
    $ids = array ();
    @$newids = scandir ($folder);
    if ($newids) {
      $newids = Filter_Folders::cleanup ($newids);
      foreach ($newids as $newid) {
        $file = $this->userOldIDFile ($username, $bible, $book, $chapter, $newid);
        $oldid = file_get_contents ($file);
        $ids [] = array ("oldid" => $oldid, "newid" => $newid);
      }
    }
    return $ids;
  }


  public function getUserChapter ($username, $bible, $book, $chapter, $newID)
  {
    $oldfile = $this->userOldTextFile ($username, $bible, $book, $chapter, $newID);
    $newfile = $this->userNewTextFile ($username, $bible, $book, $chapter, $newID);
    $oldtext = file_get_contents ($oldfile);
    $newtext = file_get_contents ($newfile);
    $result = array ("oldtext" => $oldtext, "newtext" => $newtext);
    return $result;
  }


  public function getUserTimestamp ($username, $bible, $book, $chapter, $newID)
  {
    $file = $this->userTimeFile ($username, $bible, $book, $chapter, $newID);
    $time = file_get_contents ($file);
    if ($time) return $time;
    return time ();
  }


  // Code dealing with the notifications.
 

  private function notificationsMainFolder ()
  {
    return realpath (__DIR__ . "/../databases/modifications/notifications");
  }
  
  
  private function notificationIdentifierFolder ($identifier)
  {
    return $this->notificationsMainFolder () . "/$identifier";
  }
  
  
  public function notificationTimeFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/time";
  }


  private function notificationUserFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/user";
  }


  private function notificationCategoryFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/category";
  }


  private function notificationBibleFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/bible";
  }


  private function notificationPassageFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/passage";
  }


  private function notificationOldtextFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/oldtext";
  }


  private function notificationModificationFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/modification";
  }


  private function notificationNewtextFile ($identifier)
  {
    return $this->notificationIdentifierFolder ($identifier) . "/newtext";
  }


  public function getNextAvailableNotificationIdentifier ()
  {
    // Read the existing identifiers in the folder.
    $files = scandir ($this->notificationsMainFolder ());
    $files = Filter_Folders::cleanup ($files);
    // Sort from low to high.
    sort ($files, SORT_NUMERIC);
    // Fetch the last and highest identifier.
    @$identifier = array_pop ($files);
    if (!$identifier) $identifier = 0;
    // Take next one.
    $identifier++;
    // Done.
    return $identifier;
  }


  public function recordNotification ($users, $category, $bible, $book, $chapter, $verse, $oldtext, $modification, $newtext)
  {
    if ($verse == "") $verse = 0;
    // Normally this function is called just after midnight.
    // It would then put the current time on changes made the day before.
    // Make a correction for that by subtracting 6 hours.
    $timestamp = time () - 21600;
    foreach ($users as $user) {
      $identifier = $this->getNextAvailableNotificationIdentifier ();
      mkdir ($this->notificationIdentifierFolder ($identifier));
      $file = $this->notificationTimeFile ($identifier);
      file_put_contents ($file, $timestamp);
      $file = $this->notificationUserFile ($identifier);
      file_put_contents ($file, $user);
      $file = $this->notificationCategoryFile ($identifier);
      file_put_contents ($file, $category);
      $file = $this->notificationBibleFile ($identifier);
      file_put_contents ($file, $bible);
      $passage = "$book.$chapter.$verse";
      $file = $this->notificationPassageFile ($identifier);
      file_put_contents ($file, $passage);
      $file = $this->notificationOldtextFile ($identifier);
      file_put_contents ($file, $oldtext);
      $file = $this->notificationModificationFile ($identifier);
      file_put_contents ($file, $modification);
      $file = $this->notificationNewtextFile ($identifier);
      file_put_contents ($file, $newtext);
    }
  }
  
  
  public function indexTrimAllNotifications ()
  {
    // Delete the index database and create an empty one.
    $this->delete ();
    $this->create ();

    // Change notifications expire after 30 days.
    $expiry_time = time () - 2592000;

    // Database: Connect and speed it up.
    $db = $this->connect ();
    Database_SQLite::exec ($db, "PRAGMA synchronous = OFF;");

    // Go through the notifications on disk.
    $identifiers = scandir ($this->notificationsMainFolder ());
    $identifiers = Filter_Folders::cleanup ($identifiers);
    sort ($identifiers, SORT_NUMERIC);
    foreach ($identifiers as $identifier) {

      // Fetch the data from the filesystem.
      $timestamp = file_get_contents ($this->notificationTimeFile ($identifier));
      $user = file_get_contents ($this->notificationUserFile ($identifier));
      $category = file_get_contents ($this->notificationCategoryFile ($identifier));
      $bible = file_get_contents ($this->notificationBibleFile ($identifier));
      $passage = file_get_contents ($this->notificationPassageFile ($identifier));
      $passage = explode (".", $passage);
      @$book = $passage [0];
      @$chapter = $passage [1];
      @$verse = $passage [2];
      $modification = $this->getNotificationModification ($identifier);

      // Validate the data.
      $valid = true;
      if ($timestamp < $expiry_time) $valid = false;
      if ($user == "") $valid = false;
      if ($bible == "") $valid = false;
      if (empty ($passage)) $valid = false;
      if (!$book) $valid = false;
      if ($chapter == "") $valid = false;
      if ($verse == "") $valid = false;
      if ($modification == "") $valid = false;
     
      if ($valid) {
        // Store valid data in the database.
        $identifier = Database_SQLiteInjection::no ($identifier);
        $timestamp = Database_SQLiteInjection::no ($timestamp);
        $user = Database_SQLiteInjection::no ($user);
        $category = Database_SQLiteInjection::no ($category);
        $bible = Database_SQLiteInjection::no ($bible);
        $book = Database_SQLiteInjection::no ($book);
        $chapter = Database_SQLiteInjection::no ($chapter);
        $verse = Database_SQLiteInjection::no ($verse);
        $modification = Database_SQLiteInjection::no ($modification);
        $query = "INSERT INTO notifications VALUES ($identifier, $timestamp, '$user', '$category', '$bible', $book, $chapter, $verse, '$modification');";
        Database_SQLite::exec ($db, $query);
      } else {
        // Delete invalid / expired data.
        $this->deleteNotification ($identifier, $db);
      }
    }
    
    unset ($db);
  }
  
  
  public function getNotificationIdentifiers ($username = "", $limit = false)
  {
    $db = $this->connect ();
    $ids = array ();
    $query = "SELECT identifier FROM notifications ";
    if ($username != "") {
      $username = Database_SQLiteInjection::no ($username);
      $query .= " WHERE username = '$username' ";
    }
    // Sort on reference, so that related change notifications are near each other.
    $query .= " ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC";
    // Optionally limit the hits.
    if ($limit) {
      $query .= " LIMIT 1000";
    }
    // Run it.
    $query .= " ;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $ids [] = $row [0];
    }
    unset ($db);
    return $ids;
  }


  // This gets the identifiers of the personal change proposals.
  // For easier comparison, it also gets the identifiers of the changes
  // in the verses that have personal change proposals.
  public function getNotificationPersonalIdentifiers ($username, $category, $limit = false)
  {
    $db = $this->connect ();
    $username = Database_SQLiteInjection::no ($username);

    // Get all personal change proposals.
    $personalIDs = array ();
    $query = "SELECT identifier FROM notifications WHERE username = '$username' AND category = '$category' ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC";
    // Optionally limit the hits.
    if ($limit) {
      $query .= " LIMIT 1000";
    }
    // Run it.
    $query .= " ;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $personalIDs [] = $row [0];
    }

    $allIDs = array ();

    // Go through each of the personal change proposals.
    foreach ($personalIDs as $personalID) {
      // Add the personal change proposal to the results.
      $allIDs [] = $personalID;
      // Get the Bible and passage for this change proposal.
      $bible = $this->getNotificationBible ($personalID);
      $passage = $this->getNotificationPassage ($personalID);
      $book = $passage ['book'];
      $chapter = $passage ['chapter'];
      $verse = $passage ['verse'];
      // Look for change proposals for this Bible and passage.
      $query = "SELECT identifier FROM notifications WHERE username = '$username' AND bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse ORDER BY identifier ASC;";
      $result = Database_SQLite::query ($db, $query);
      foreach ($result as $row) {
        $id = $row [0];
        // Add the identifier if it's not yet in.
        if (!in_array ($id, $allIDs)) {
          $allIDs [] = $id;
        }
      }
    }
    
    unset ($db);

    return $allIDs;
  }


  // This gets the identifiers of the team's changes.
  public function getNotificationTeamIdentifiers ($username, $category, $limit = false)
  {
    $db = $this->connect ();
    $username = Database_SQLiteInjection::no ($username);
    $ids = array ();
    $query = "SELECT identifier FROM notifications WHERE username = '$username' AND category = '$category' ORDER BY book ASC, chapter ASC, verse ASC, identifier ASC";
    // Optionally limit the hits.
    if ($limit) {
      $query .= " LIMIT 1000";
    }
    // Run it.
    $query .= " ;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $ids [] = $row [0];
    }
    unset ($db);
    return $ids;
  }


  public function deleteNotification ($identifier, $db = null)
  {
    // Delete from the filesystem.
    $folder = $this->notificationIdentifierFolder ($identifier);
    Filter_Rmdir::rmdir ($folder);
    // Delete from the database.
    $identifier = Database_SQLiteInjection::no ($identifier);
    $query = "DELETE FROM notifications WHERE identifier = $identifier;";
    // Make a very short connection to the database, 
    // to prevent corruption when a user deletes masses of changes notifications 
    // by keeping the delete key pressed.
    if ($db == null) $db = $this->connect ();
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  public function getNotificationTimeStamp ($id)
  {
    $db = $this->connect ();
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT timestamp FROM notifications WHERE identifier = $id;";
    $result = Database_SQLite::query ($db, $query);
    $time = time ();
    foreach ($result as $row) {
      $time = $row [0];
    }
    unset ($db);
    return $time;
  }


  public function getNotificationCategory ($id)
  {
    $db = $this->connect ();
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT category FROM notifications WHERE identifier = $id;";
    $result = Database_SQLite::query ($db, $query);
    $category = NULL;
    foreach ($result as $row) {
      $category = $row [0];
    }
    unset ($db);
    return $category;
  }


  public function getNotificationBible ($id)
  {
    $db = $this->connect ();
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT bible FROM notifications WHERE identifier = $id;";
    $result = Database_SQLite::query ($db, $query);
    $bible = NULL;
    foreach ($result as $row) {
      $bible = $row [0];
    }
    unset ($db);
    return $bible;
  }


  public function getNotificationPassage ($id)
  {
    $passage = NULL;
    $db = $this->connect ();
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT book, chapter, verse FROM notifications WHERE identifier = $id;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      unset ($row[2]);
      $passage = $row;
    }
    unset ($db);
    return $passage;
  }


  public function getNotificationOldText ($id)
  {
    $file = $this->notificationOldtextFile ($id);
    @$oldtext = file_get_contents ($file);
    if (!$oldtext) return "";
    return $oldtext;
  }


  public function getNotificationModification ($id)
  {
    $file = $this->notificationModificationFile ($id);
    @$modification = file_get_contents ($file);
    if (!$modification) return NULL;
    return $modification;
  }


  public function getNotificationNewText ($id)
  {
    $file = $this->notificationNewtextFile ($id);
    @$newtext = file_get_contents ($file);
    if (!$newtext) return "";
    return $newtext;
  }


  public function clearNotificationsUser ($username)
  {
    $identifiers = $this->getNotificationIdentifiers ($username);
    $db = $this->connect ();
    // Transaction speeds up the operation.
    Database_SQLite::exec ($db, "BEGIN;");
    foreach ($identifiers as $identifier) {
      $this->deleteNotification ($identifier, $db);
    }
    Database_SQLite::exec ($db, "COMMIT;");
    unset ($db);
  }


  // This function deletes personal change proposals and their matching change notifications.
  public function clearNotificationMatches ($username, $personal, $team)
  {
    $db = $this->connect ();
    
    // Clean input.
    $personal = Database_SQLiteInjection::no ($personal);
    $team = Database_SQLiteInjection::no ($team);

    // Select all identifiers of the personal change proposals.
    $query = "SELECT identifier FROM notifications WHERE username = '$username' AND category = '$personal';";

    $personals = array ();
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $personals [] = $row [0];
    }
    
    // Matches to be deleted.
    $deletes = array ();

    // Go through each of the personal change proposals.
    foreach ($personals as $personalID) {
      $bible = $this->getNotificationBible ($personalID);
      $passage = $this->getNotificationPassage ($personalID);
      $book = $passage ['book'];
      $chapter = $passage ['chapter'];
      $verse = $passage ['verse'];
      $modification = $this->getNotificationModification ($personalID);
      $modification = Database_SQLiteInjection::no ($modification);
      // Get all matching identifiers from the team's change notifications.
      $query = "SELECT identifier FROM notifications WHERE username = '$username' AND category = '$team' AND bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse AND modification = '$modification';";
      $teamMatches = array ();
      $result = Database_SQLite::query ($db, $query);
      foreach ($result as $row) {
        $teamMatches [] = $row [0];
      }
      // There should be exactly one candidate for the matches to be removed.
      // If there are none, it means that the personal change didn't make it to the team's text.
      // If there are two or more matching changes, then one could have undone the other, so should not be automatically removed.
      if (count ($teamMatches) == 1) {
        foreach ($teamMatches as $teamMatches) {
          // Check there are only two change notifications for this user / Bible / book / chapter / verse.
          // If there are more, we can't be sure that the personal change proposal was not overwritten somehow.
          $passageMatches = array ();
          $query = "SELECT identifier FROM notifications WHERE username = '$username' AND bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse;";
          $result = Database_SQLite::query ($db, $query);
          foreach ($result as $row) {
            $passageMatches [] = $row [0];
          }
          if (count ($passageMatches) == 2) {
            // Store the personal change proposal to be deleted.
            // Store the matching change notification to be deleted also.
            foreach ($passageMatches as $passageMatche) {
              $deletes [] = $passageMatche;
            }
          }
        }
      }
    }

    // Delete all stored identifiers to be deleted.
    foreach ($deletes as $delete) {
      $this->deleteNotification ($delete, $db);
    }
    
    unset ($db);
  }


}


?>
