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


// Database resilience.
// The notes are stored in the plain filesystem for robustness.
// A database can easily be corrupted. The filesystem is more robust.
// The notes database works like this:
// * All read operations are done from the filesystem.
// * All write operations first go to the file system, then to the database.
// * All search and locate operations work through the database.
// * Connections to the database are alive as short as possible.


class Database_Notes
{


  private static $instance;
  private $standard_statuses = array ("New", "Pending", "In progress", "Done", "Reopened");
  private $standard_severities = array (0 => "Wish", 1 => "Minor", 2 => "Normal", 3 => "Important", 4 => "Major", 5 => "Critical");


  private function __construct ()
  {
  }


  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Notes();
    }
    // Enter the standard statuses in the list of translatable strings.
    if (false) {
      Locale_Translate::_("New");
      Locale_Translate::_("Pending");
      Locale_Translate::_("In progress");
      Locale_Translate::_("Done");
      Locale_Translate::_("Reopened");
    }
    // Enter the standard severities in the list of translatable strings.
    if (false) {
      Locale_Translate::_("Wish");
      Locale_Translate::_("Minor");
      Locale_Translate::_("Normal");
      Locale_Translate::_("Important");
      Locale_Translate::_("Major");
      Locale_Translate::_("Critical");
    }
    return self::$instance;
  }


  private function connect ()
  {
    $db = Database_SQLite::connect ("notes");
    return $db;
  }
  

  private function connect_checksums ()
  {
    $db = Database_SQLite::connect ("notes_checksums");
    return $db;
  }
  

  public function create ()
  {
    // Create the main database and table.
    $db = self::connect ();
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS notes (
  id integer primary key autoincrement,
  identifier integer NOT NULL,
  modified integer NOT NULL,
  assigned text,
  subscriptions text,
  bible text,
  passage text,
  status text,
  severity integer,
  summary text,
  contents text,
  cleantext text
);
EOD;
    Database_SQLite::exec ($db, $sql);
    unset ($db);

    // Create the database and table for the checksums.
    // A general reason for having this separate is robustness.
    // A specific reason for this is that when the main notes database is being repaired,
    // and several clients keep reading it, it may disrupt the repair.
    $db = self::connect_checksums ();
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS checksums (
  identifier integer,
  checksum checksum
);
EOD;
    Database_SQLite::exec ($db, $sql);
    unset ($db);
  }


  public function database_path ()
  {
    $file = "../databases/notes.sqlite";
    $path = realpath (__DIR__ . "/" . $file);
    return $path;
  }
  

  public function checksums_database_path ()
  {
    $file = "../databases/notes_checksums.sqlite";
    $path = realpath (__DIR__ . "/" . $file);
    return $path;
  }
  

  // Returns whether the notes database is healthy, as a boolean.
  public function healthy ()
  {
    // Do an integrity check to find out whether the notes database is healthy.
    // An empty file appears healthy too, so deal with that.
    $ok = false;
    if (filesize ($this->database_path ()) > 0) {
      $db = self::connect ();
      $query = "PRAGMA integrity_check;";
      $result = Database_SQLite::query ($db, $query);
      $ok = false;
      foreach ($result as $row) {
        if ($row[0] == "ok") $ok = true;
      }
      unset ($db);
    }
    return $ok;
  }


  // Returns whether the notes checksums database is healthy, as a boolean.
  public function checksums_healthy ()
  {
    // Do an integrity check to find out whether the notes checksums database is healthy.
    // An empty file appears healthy too, so deal with that.
    $ok = false;
    if (filesize ($this->checksums_database_path ()) > 0) {
      $db = self::connect_checksums ();
      $query = "PRAGMA integrity_check;";
      $result = Database_SQLite::query ($db, $query);
      $ok = false;
      foreach ($result as $row) {
        if ($row[0] == "ok") $ok = true;
      }
      unset ($db);
    }
    return $ok;
  }
  
  
  // Does a checkup on the health of the main database.
  // Optionally recreates it.
  // Returns true if to be synced, else false.
  public function checkup ()
  {
    if ($this->healthy ()) return false;

    $database_path = $this->database_path ();
    unlink ($database_path);

    $this->create ();

    return true;
  }


  // Does a checkup on the health of the checksums database.
  // Optionally recreates it.
  // Returns true if to synced, else false.
  public function checkup_checksums ()
  {
    if ($this->checksums_healthy ()) return false;

    $database_path = $this->checksums_database_path ();
    unlink ($database_path);

    $this->create ();

    return true;
  }


  public function trim ()
  {
    // Clean empty directories.
    $mainFolder = $this->mainFolder ();
    $bits1 = scandir ($mainFolder);
    $bits1 = Filter_Folders::cleanup ($bits1);
    foreach ($bits1 as $bit1) {
      if (is_numeric ($bit1)) {
        $folder = "$mainFolder/$bit1";
        $bits2 = scandir ($folder);
        $bits2 = Filter_Folders::cleanup ($bits2);
        if (empty ($bits2)) {
          Filter_Rmdir::rmdir ($folder);
        }
        foreach ($bits2 as $bit2) {
          if (is_numeric ($bit2)) {
            $folder = "$mainFolder/$bit1/$bit2";
            $bits3 = scandir ($folder);
            $bits3 = Filter_Folders::cleanup ($bits3);
            if (empty ($bits3)) {
              Filter_Rmdir::rmdir ($folder);
            }
          }
        }
      }
    }
  }


  public function trim_server ()
  {
    // Notes expiry.
    $this->touchMarkedForDeletion ();
    $identifiers = $this->getDueForDeletion ();
    foreach ($identifiers as $identifier) {
      $this->delete ($identifier);
    }
  }


  public function optimize ()
  {
    $db = self::connect ();
    Database_SQLite::exec ($db, "VACUUM notes;");
    unset ($db);
  }


  public function sync ()
  {
    $mainFolder = $this->mainFolder ();

    // List of notes in the filesystem.
    $identifiers = array ();

    $bits1 = scandir ($mainFolder);
    foreach ($bits1 as $bit1) {
      if (is_numeric ($bit1)) {
        $bits2 = scandir ("$mainFolder/$bit1");
        foreach ($bits2 as $bit2) {
          if (is_numeric ($bit2)) {
            $bits3 = scandir ("$mainFolder/$bit1/$bit2");
            foreach ($bits3 as $bit3) {
              if (is_numeric ($bit3)) {
                $identifier = "$bit1$bit2$bit3";
                $identifiers [] = $identifier;
                $this->updateDatabase ($identifier);
                $this->updateSearchFields ($identifier);
                $this->updateChecksum ($identifier);
              }
            }
          }
        }
      }
    }

    // Get all identifiers in the main notes index.
    $db = self::connect ();
    $database_identifiers = array ();
    $query = "SELECT identifier FROM notes;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $database_identifiers [] = $row[0];
    }
    unset ($db);

    // Any note identifiers in the main index, and not in the filesystem, remove them.
    $deletes = array_diff ($database_identifiers, $identifiers);
    foreach ($deletes as $identifier) {
      $this->delete ($identifier);
    }
    
    // Get all identifiers in the checksums database.
    $db = self::connect_checksums ();
    $database_identifiers = array ();
    $query = "SELECT identifier FROM checksums;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $database_identifiers [] = $row[0];
    }
    unset ($db);

    // Any note identifiers in the checksums database, and not in the filesystem, remove them.
    $deletes = array_diff ($database_identifiers, $identifiers);
    foreach ($deletes as $identifier) {
      $this->deleteChecksum ($identifier);
    }

    
  }


  public function updateDatabase ($identifier)
  {
    // Read the relevant values from the filesystem.
    $modified = $this->getModified ($identifier);

    $file = $this->assignedFile ($identifier);
    @$assigned = file_get_contents ($file);
    if (!$assigned) $assigned = "";

    $file = $this->subscriptionsFile ($identifier);
    @$subscriptions = file_get_contents ($file);
    if (!$subscriptions) $subscriptions = "";

    $bible = $this->getBible ($identifier);

    $passage = $this->getRawPassage ($identifier);

    $status = $this->getRawStatus ($identifier);

    $severity = $this->getRawSeverity ($identifier);

    $summary = $this->getSummary ($identifier);

    $contents = $this->getContents ($identifier);

    // Security.
    $identifier = Database_SQLiteInjection::no ($identifier);

    // Read the relevant values from the database.
    // If all the values in the database are the same as the values on the filesystem,
    // it means that the database is already in sync with the filesystem.
    // Bail out in that case.
    $db = self::connect ();
    $database_in_sync = true;
    $record_in_database = false;
    $query = "SELECT modified, assigned, subscriptions, bible, passage, status, severity, summary, contents FROM notes WHERE identifier = $identifier;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $record_in_database = true;
      if ($modified != $row ["modified"]) $database_in_sync = false;
      if ($assigned != $row ["assigned"]) $database_in_sync = false;
      if ($subscriptions != $row ["subscriptions"]) $database_in_sync = false;
      if ($bible != $row ["bible"]) $database_in_sync = false;
      if ($passage != $row ["passage"]) $database_in_sync = false;
      if ($status != $row ["status"]) $database_in_sync = false;
      if ($severity != $row ["severity"]) $database_in_sync = false;
      if ($summary != $row ["summary"]) $database_in_sync = false;
      if ($contents != $row ["contents"]) $database_in_sync = false;
    }
    unset ($db);
    if ($database_in_sync && $record_in_database) return;

    // At this stage, the index needs to be brought in sync with the filesystem.
    $modified = Database_SQLiteInjection::no ($modified);
    $assigned = Database_SQLiteInjection::no ($assigned);
    $subscriptions = Database_SQLiteInjection::no ($subscriptions);
    $bible = Database_SQLiteInjection::no ($bible);
    $passage = Database_SQLiteInjection::no ($passage);
    $status = Database_SQLiteInjection::no ($status);
    $severity = Database_SQLiteInjection::no ($severity);
    $summary = Database_SQLiteInjection::no ($summary);
    $contents = Database_SQLiteInjection::no ($contents);

    $db = self::connect ();

    $query = "DELETE FROM notes WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);

    $query = "INSERT INTO notes (identifier, modified, assigned, subscriptions, bible, passage, status, severity, summary, contents) VALUES ($identifier, $modified, '$assigned', '$subscriptions', '$bible', '$passage', '$status', $severity, '$summary', '$contents')";
    Database_SQLite::exec ($db, $query);

    unset ($db);
  }
  
  
  public function mainFolder ()
  {
    $folder = realpath (__DIR__ . "/../consultations");
    return $folder;
  }
  
  
  public function noteFolder ($identifier)
  {
    // The maximum number of folders a folder may contain is limited.
    // To overcome this limit, the notes are stored in a deep folder structure.
    $bit1 = substr ($identifier, 0, 3);
    $bit2 = substr ($identifier, 3, 3);
    $bit3 = substr ($identifier, 6, 3);
    $folder = $this->mainFolder () . "/$bit1/$bit2/$bit3";
    return $folder;
  }


  public function bibleFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/bible";
    return $file;
  }


  public function passageFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/passage";
    return $file;
  }


  public function statusFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/status";
    return $file;
  }


  public function severityFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/severity";
    return $file;
  }


  public function modifiedFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/modified";
    return $file;
  }


  public function summaryFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/summary";
    return $file;
  }


  public function contentsFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/contents";
    return $file;
  }
  
  
  public function subscriptionsFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/subscriptions";
    return $file;
  }


  public function assignedFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/assigned";
    return $file;
  }


  private function expiryFile ($identifier)
  {
    $file = $this->noteFolder ($identifier) . "/expiry";
    return $file;
  }


  // This checks whether the note identifier exists.
  public function identifierExists ($identifier)
  {
    return file_exists ($this->noteFolder ($identifier));
  }


  // Update a note's $identifier.
  // $new_identifier is the value given to the note identifier by $identifier.
  public function setIdentifier ($identifier, $new_identifier)
  {
    // Move data on the filesystem.
    $this->delete ($new_identifier);
    $file = $this->noteFolder ($identifier);
    $newfile = $this->noteFolder ($new_identifier);
    @mkdir (dirname ($newfile), 0777, true);
    rename ($file, $newfile);
    
    $identifier = Database_SQLiteInjection::no ($identifier);
    $new_identifier = Database_SQLiteInjection::no ($new_identifier);

    // Update main notes database.
    $db = self::connect ();
    $query = "UPDATE notes SET identifier = $new_identifier WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    
    // Update checksums database.
    $db = self::connect_checksums ();
    $query = "UPDATE checksums SET identifier = $new_identifier WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    
  }


  public function getNewUniqueIdentifier ()
  {
    do {
      $identifier = rand (Notes_Logic::lowNoteIdentifier, Notes_Logic::highNoteIdentifier);
    } while ($this->identifierExists ($identifier));
    return $identifier;
  }


  public function getIdentifiers ()
  {
    $db = self::connect ();
    $identifiers = array ();
    $query = "SELECT identifier FROM notes;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $identifiers [] = $row[0];
    }
    unset ($db);
    return $identifiers;
  }


  private function assembleContents ($identifier, $contents)
  {
    $new_contents = "";
    if (is_numeric ($identifier)) {
      $new_contents = $this->getContents ($identifier);
    }
    $datetime = new DateTime();
    Filter_Datetime::user_zone ($datetime);
    $datetime = $datetime->format(DATE_RSS);
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $new_contents .= "<p>$user ($datetime):</p>\n";
    if ($contents == "<br>") $contents = "";
    $lines = explode ("\n", $contents);
    foreach ($lines as $line) {
      $line = trim ($line);
      $new_contents .= "<p>$line</p>\n";
    }
    return $new_contents;
  }


  // Store a new note into the database.
  // $bible: The notes's Bible.
  // $book, $chapter, $verse: The note's passage.
  // $summary: The note's summary.
  // $contents: The note's contents.
  // $raw: Import $contents as it is. Useful for import from Bibledit-Gtk.
  // It returns the $identifier of this new note.
  public function storeNewNote ($bible, $book, $chapter, $verse, $summary, $contents, $raw)
  {
    // Create a new identifier.
    $identifier = $this->getNewUniqueIdentifier ();

    // Passage.
    $passage = $this->encodePassage ($book, $chapter, $verse);
    
    $status = 'New';
    $severity = 2;

    // If the $summary is not given, take the first line of the $contents as the $summary.
    if ($summary == "") {
      // The notes editor does not put new lines at each line, but instead <div>s. Handle these also.
      $summary = str_replace ("<", "\n", $contents);
      $summary = explode ("\n", $summary);
      $summary = $summary[0];
    }

    // Assemble contents.
    if (!$raw) $contents = $this->assembleContents ($identifier, $contents);
    if (($contents == "") && ($summary == "")) return;
    
    // Store the note in the file system.
    mkdir ($this->noteFolder ($identifier), 0777, true);
    file_put_contents ($this->bibleFile ($identifier), $bible);
    file_put_contents ($this->passageFile ($identifier), $passage);
    file_put_contents ($this->statusFile ($identifier), $status);
    file_put_contents ($this->severityFile ($identifier), $severity);
    file_put_contents ($this->summaryFile ($identifier), $summary);
    file_put_contents ($this->contentsFile ($identifier), $contents);
    
    // Store new default note into the database.
    $db = self::connect ();
    $bible = Database_SQLiteInjection::no ($bible);
    $passage = Database_SQLiteInjection::no ($passage);
    $summary = Database_SQLiteInjection::no ($summary);
    $contents = Database_SQLiteInjection::no ($contents);
    $query = "INSERT INTO notes (identifier, modified, assigned, subscriptions, bible, passage, status, severity, summary, contents) VALUES ($identifier, 0, '', '', '$bible', '$passage', '$status', $severity, '$summary', '$contents')";
    Database_SQLite::exec ($db, $query);
    unset ($db);

    // Updates.
    $this->updateSearchFields ($identifier);
    $this->noteEditedActions ($identifier);

    // Return this new note´s identifier.
    return $identifier;
  }


  // Returns an array of note identifiers selected.
  // $bibles: Array of Bible names the user has read access to. 
  //          Or NULL to disable selection on Bibles.
  // $book, $chapter, $verse, $passage_selector: These are related and can limit the selection.
  // $edit_selector: Optionally constrains selection based on modification time.
  // $non_edit_selector: Optionally constrains selection based on modification time.
  // $status_selector: Optionally constrains selection based on note status.
  // $bible_selector: Optionally constrains the selection, based on the note's Bible.
  // $assignment_selector: Optionally constrains the selection based on a note being assigned to somebody.
  // $subscription_selector: Optionally limits the selection based on a note's subscription.
  // $severity_selector: Optionally limits the selection, based on a note's severity.
  // $text_selector: Optionally limits the selection to notes that contains certain text. Used for searching notes.
  // $search_text: Works with $text_selector, contains the text to search for.
  // $limit: If non-NULL, it indicates the starting limit for the selection.
  public function selectNotes ($bibles, $book, $chapter, $verse, $passage_selector, $edit_selector, $non_edit_selector, $status_selector, $bible_selector, $assignment_selector, $subscription_selector, $severity_selector, $text_selector, $search_text, $limit)
  {
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $identifiers = array ();
    // SQL SELECT statement.
    $query = Filter_Sql::notesSelectIdentifier ();
    // SQL optional fulltext search statement sorted on relevance.
    if ($text_selector == 1) {
      $query .= Filter_Sql::notesOptionalFulltextSearchRelevanceStatement ($search_text);
    }
    // SQL FROM ... WHERE statement.
    $query .= Filter_Sql::notesFromWhereStatement ();
    // Consider passage selector.
    switch ($passage_selector) {
      case 0:
        // Select notes that refer to the current verse.
        // It means that the book, the chapter, and the verse, should match.
        $passage = $this->encodePassage ($book, $chapter, $verse);
        $query .= " AND passage LIKE '%$passage%' ";
        break;
      case 1:
        // Select notes that refer to the current chapter.
        // It means that the book and the chapter should match.
        $passage = $this->encodePassage ($book, $chapter, NULL);
        $query .= " AND passage LIKE '%$passage%' ";
        break;
      case 2:
        // Select notes that refer to the current book.
        // It means that the book should match.
        $passage = $this->encodePassage ($book, NULL, NULL);
        $query .= " AND passage LIKE '%$passage%' ";
        break;
      case 3:
        // Select notes that refer to any passage: No constraint to apply here.
        break;
    }
    // Consider edit selector.
    switch ($edit_selector) {
      case 0:
        // Select notes that have been edited at any time. Apply no constraint.
        $time = 0;
        break;
      case 1:
        // Select notes that have been edited during the last 30 days.
        $time = strtotime ("today -30 days");
        break;
      case 2:
        // Select notes that have been edited during the last 7 days.
        $time = strtotime ("today -7 days");
        break;
      case 3:
        // Select notes that have been edited since yesterday.
        $time = strtotime ("yesterday");
        break;
      case 4:
        // Select notes that have been edited today.
        $time = strtotime ("today");
        break;
    }
    if ($time != 0) $query .= " AND modified >= $time ";
    // Consider non-edit selector.
    switch ($non_edit_selector) {
      case 0:
        // Select notes that have not been edited at any time. Apply no constraint.
        $nonedit = 0;
        break;
      case 1:
        // Select notes that have not been edited for a day.
        $nonedit = strtotime ("-1 day");
        break;
      case 2:
        // Select notes that have not been edited for two days.
        $nonedit = strtotime ("-2 days");
        break;
      case 3:
        // Select notes that have not been edited for a week.
        $nonedit = strtotime ("-1 week");
        break;
      case 4:
        // Select notes that have not been edited today.
        $nonedit = strtotime ("-1 month");
        break;
      case 5:
        // Select notes that have not been edited today.
        $nonedit = strtotime ("-1 year");
        break;
    }
    if ($nonedit != 0) $query .= " AND modified <= $nonedit ";
    // Consider status constraint.
    if ($status_selector != "") {
      $query .= " AND status = '$status_selector' ";
    }
    // Consider Bible constraints:
    // * A user has access to notes that refer to Bibles the user has access to.
    // * If a $bible_selector is given: Select notes that refer to this Bible.
    // * A note can be a general one, not referring to any specific Bible.
    //   Select such notes also.
    if (is_array ($bibles)) {
      if ($bible_selector != "") $bibles = array ($bible_selector);
      $query .= " AND (bible = '' ";
      foreach ($bibles as $bible) {
        $bible = Database_SQLiteInjection::no ($bible);
        $query .= " OR bible = '$bible' ";
      }
      $query .= " ) ";
    }
    // Consider note assignment constraints.
    if ($assignment_selector != "") {
      $assignment_selector = Database_SQLiteInjection::no ($assignment_selector);
      $query .= " AND assigned LIKE '% $assignment_selector %' ";
    }
    // Consider note subscription constraints.
    if ($subscription_selector == 1) {
      $query .= " AND subscriptions LIKE '% $username %' ";
    }
    // Consider the note severity.
    if ($severity_selector != -1) {
      $query .= " AND severity = $severity_selector ";
    }
    // Consider text contained in notes.
    if ($text_selector == 1) {
      $query .= Filter_Sql::notesOptionalFulltextSearchStatement ($search_text);
    }
    if ($text_selector == 1) {
      // If searching in fulltext mode, notes get ordered on relevance of search hits.
      $query .= Filter_Sql::notesOrderByRelevanceStatement ();
    } else {
      // Notes get ordered by the passage they refer to. It is a rough method and better ordering is needed.
      $query .= " ORDER BY ABS (passage) ";
    }
    // Limit the selection if a limit is given.
    if (is_numeric ($limit)) {
      $limit = Database_SQLiteInjection::no ($limit);
      $query .= " LIMIT $limit, 50 ";
    }
    $query .= ";";

    $db = self::connect ();
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $identifiers []= $row[0];
    }
    unset ($db);
    return $identifiers;
  }


  public function getSummary ($identifier)
  {
    $file = $this->summaryFile ($identifier);
    @$summary = file_get_contents ($file);
    if ($summary) return $summary;
    return "";
  }


  public function setSummary ($identifier, $summary)
  {
    // Store authoritative copy in the filesystem.
    $file = $this->summaryFile ($identifier);
    file_put_contents ($file, $summary);
    // Update the shadow database.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $summary = Database_SQLiteInjection::no ($summary);
    $query = "UPDATE notes SET summary = '$summary' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    // Update the search data in the database.
    $this->updateSearchFields ($identifier);
    // Update checksum.
    $this->updateChecksum ($identifier);
  }


  public function getContents ($identifier)
  {
    $file = $this->contentsFile ($identifier);
    @$contents = file_get_contents ($file);
    if ($contents) return $contents;
    return "";
  }


  public function setContents ($identifier, $contents)
  {
    // Store in file system.
    $file = $this->contentsFile ($identifier);
    file_put_contents ($file, $contents);
    // Update database.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $contents = Database_SQLiteInjection::no ($contents);
    $query = "UPDATE notes SET contents = '$contents' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    // Update search system.
    $this->updateSearchFields ($identifier);
    // Update checksum.
    $this->updateChecksum ($identifier);
  }


  public function delete ($identifier)
  {
    // Delete from filesystem.
    $folder = $this->noteFolder ($identifier);
    Filter_Rmdir::rmdir ($folder);
    // Update database as well.
    $this->deleteChecksum ($identifier);
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $query = "DELETE FROM notes WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Add a $comment to an exiting note identified by $identifier.
  public function addComment ($identifier, $comment)
  {
    // Assemble the new content and store it.
    // This updates the search database also.
    $contents = $this->assembleContents ($identifier, $comment);
    $this->setContents ($identifier, $contents);

    // Some triggeres.
    $this->noteEditedActions ($identifier);
    $this->unmarkForDeletion ($identifier);

    // Update shadow database.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $contents = Database_SQLiteInjection::no ($contents);
    $query = "UPDATE notes SET contents = '$contents' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Subscribe the current user to the note identified by $identifier.
  public function subscribe ($identifier)
  {
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $this->subscribeUser ($identifier, $user);
  }


  // Subscribe the $user to the note identified by $identifier.
  public function subscribeUser ($identifier, $user)
  {
    // If the user already is subscribed to the note, bail out.
    $subscribers = $this->getSubscribers ($identifier);
    if (in_array ($user, $subscribers)) return;
    // Subscribe $user.
    $subscribers [] = $user;
    $this->setSubscribers ($identifier, $subscribers);
  }


  // Returns an array with the subscribers to the note identified by $identifier.
  public function getSubscribers ($identifier)
  {
    $file = $this->subscriptionsFile ($identifier);
    @$subscribers = file_get_contents ($file);
    if (!$subscribers) return array ();
    $subscribers = explode ("\n", $subscribers);
    $subscribers = array_diff ($subscribers, array (""));
    foreach ($subscribers as $offset => $subscriber) {
      $subscribers [$offset] = trim ($subscriber);
    }
    return $subscribers;
  }


  public function setSubscribers ($identifier, $subscribers)
  {
    // Add a space at both sides of the subscriber to allow for easier note selection based on note assignment.
    foreach ($subscribers as &$subscriber) {
      $subscriber = " $subscriber ";
    }
    $subscribers = implode ("\n", $subscribers);
    
    // Store them in the filesystem.
    $file = $this->subscriptionsFile ($identifier);
    file_put_contents ($file, $subscribers);
    
    // Store them in the database as well.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $subscribers = Database_SQLiteInjection::no ($subscribers);
    $query = "UPDATE notes SET subscriptions = '$subscribers' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);

    // Checksum.
    $this->updateChecksum ($identifier);
  }


  // Returns true if $user is subscribed to the note identified by $identifier.
  public function isSubscribed ($identifier, $user)
  {
    $subscribers = $this->getSubscribers ($identifier);
    return in_array ($user, $subscribers);
  }


  // Unsubscribes the currently logged in user from the note identified by $identifier.
  public function unsubscribe ($identifier)
  {
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $this->unsubscribeUser ($identifier, $user);
  }


  // Unsubscribes $user from the note identified by $identifier.
  public function unsubscribeUser ($identifier, $user)
  {
    // If the user is not subscribed to the note, bail out.
    $subscribers = $this->getSubscribers ($identifier);
    if (!in_array ($user, $subscribers)) return;
    // Unsubscribe $user.
    $subscribers = array_diff ($subscribers, array ($user));
    $this->setSubscribers ($identifier, $subscribers);
  }


  // Returns an array with all assignees to the notes selection.
  // These are the usernames to which one or more notes have been assigned.
  // This means that if no notes have been assigned to anybody, it will return an empty array.
  // Normally the authoritative copy of the notes is stored in the file system.
  // But as retrieving the assignees from the file system would be slow, 
  // this function retrieves them from the database.
  // Normally the database is in sync with the filesystem.
  public function getAllAssignees ($bibles)
  {
    $assignees = array ();
    $query = "SELECT DISTINCT assigned FROM notes WHERE bible = '' ";
    foreach ($bibles as $bible) {
      $bible = Database_SQLiteInjection::no ($bible);
      $query .= " OR bible = '$bible' ";
    }
    $query .= ";";
    $db = self::connect ();
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $names = explode ("\n", $row[0]);
      $assignees = array_merge ($assignees, $names);
    }
    unset ($db);
    $assignees = array_unique ($assignees);
    foreach ($assignees as &$assignee) {
      $assignee = trim ($assignee);
    }
    $assignees = array_diff ($assignees, array (""));
    $assignees = array_values ($assignees);
    return $assignees;
  }


  // Returns an array with the assignees to the note identified by $identifier.
  public function getAssignees ($identifier)
  {
    // Get the asssignees from the filesystem.
    $file = $this->assignedFile ($identifier);
    @$assignees = file_get_contents ($file);
    return $this->getAssigneesInternal ($assignees);
  }
  
  
  private function getAssigneesInternal ($assignees)
  {
    if (!$assignees) return array ();
    $assignees = explode ("\n", $assignees);
    $assignees = array_diff ($assignees, array (""));
    // Remove the padding space at both sides of the assignee.
    foreach ($assignees as &$assignee) {
      $assignee = trim ($assignee);
    }
    return $assignees;
  }


  // Sets the note's assignees.
  // $identifier : note identifier.
  // $assignees : array of user names.
  public function setAssignees ($identifier, $assignees)
  {
    // Add a space at both sides of the assignee to allow for easier note selection based on note assignment.
    foreach ($assignees as &$assignee) {
      $assignee = " $assignee ";
    }
    $assignees = implode ("\n", $assignees);

    // Store the assignees in the filesystem.
    $file = $this->assignedFile ($identifier);
    @file_put_contents ($file, $assignees);

    // Store the assignees in the database also.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $assignees = Database_SQLiteInjection::no ($assignees);
    $query = "UPDATE notes SET assigned = '$assignees' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    
    $this->noteEditedActions ($identifier);
  }


  // Assign the note identified by $identifier to $user.
  public function assignUser ($identifier, $user)
  {
    // If the note already is assigned to the user, bail out.
    $assignees = $this->getAssignees ($identifier);
    if (in_array ($user, $assignees)) return;
    // Assign the note to the user.
    $assignees[]= "$user";
    // Store the whole lot.
    $this->setAssignees ($identifier, $assignees);
  }


  // Returns true if the note identified by $identifier has been assigned to $user.
  public function isAssigned ($identifier, $user)
  {
    $assignees = $this->getAssignees ($identifier);
    return in_array ($user, $assignees);
  }


  // Unassigns $user from the note identified by $identifier.
  public function unassignUser ($identifier, $user)
  {
    // If the notes is not assigned to the user, bail out.
    $assignees = $this->getAssignees ($identifier);
    if (!in_array ($user, $assignees)) return;
    // Remove assigned $user.
    $assignees = array_diff ($assignees, array ($user));
    $this->setAssignees ($identifier, $assignees);
  }


  public function getBible ($identifier)
  {
    $file = $this->bibleFile ($identifier);
    @$bible = file_get_contents ($file);
    if ($bible) return $bible;
    return "";
  }


  public function setBible ($identifier, $bible)
  {
    // Write the $bible to the filesystem.
    $file = $this->bibleFile ($identifier);
    file_put_contents ($file, $bible);

    // Update the database also.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "UPDATE notes SET bible = '$bible' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);

    $this->noteEditedActions ($identifier);
  }


  // Encodes the book, chapter and verse, like to, e.g.: "40.5.13",
  // and returns this as a string.
  public function encodePassage ($book, $chapter, $verse)
  {
    // Space before and after the passage enables notes selection on passage.
    // Special way of encoding, as done below, is to enable note selection on book / chapter / verse.
    $passage = " $book.";
    if ($chapter != "") $passage .= "$chapter.";
    if ($verse != "") $passage .= "$verse ";
    return $passage;
  }


  // Takes the $passage as a string, and returns an array with book, chapter, and verse.
  public function decodePassage ($passage)
  {
    $passage = trim ($passage);
    return explode (".", $passage);
  }


  // Returns the raw passage text of the note identified by $identifier.
  public function getRawPassage ($identifier)
  {
    $file = $this->passageFile ($identifier);
    @$contents = file_get_contents ($file);
    if ($contents) return $contents;
    return "";
  }


  // Returns an array with the passages that the note identified by $identifier refers to.
  // Each passages is an array (book, chapter, verse).
  public function getPassages ($identifier)
  {
    $contents = $this->getRawPassage ($identifier);
    if (!$contents) return array ();
    $lines = explode ("\n", $contents);
    $lines = array_diff ($lines, array (""));
    $passages = array ();
    foreach ($lines as $line) {
      $passage = $this->decodePassage ($line);
      $passages[] = $passage;
    }
    return $passages;
  }


  // Set the passages for note $identifier.
  // $passages is an array of an array (book, chapter, verse) passages.
  // $import: If true, just write passages, no further actions.
  public function setPassages ($identifier, $passages, $import = false)
  {
    // Format the passages.
    $line = "";
    foreach ($passages as $passage) {
      $line .= $this->encodePassage ($passage[0], $passage[1], $passage[2]);
      $line .= "\n";
    }

    // Store the authoritative copy in the filesystem.
    $file = $this->passageFile ($identifier);
    file_put_contents ($file, $line);

    if (!$import) $this->noteEditedActions ($identifier);

    // Update the shadow database also.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $line = Database_SQLiteInjection::no ($line);
    $query = "UPDATE notes SET passage = '$line' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Gets the raw status of a note.
  // Returns it as a string.
  public function getRawStatus ($identifier)
  {
    $file = $this->statusFile ($identifier);
    @$status = file_get_contents ($file);
    if ($status) return $status;
    return "";
  }


  // Gets the localized status of a note.
  // Returns it as a string.
  public function getStatus ($identifier)
  {
    $status = $this->getRawStatus ($identifier);
    // Localize status if it is a standard one.
    if (in_array ($status, $this->standard_statuses)) $status = Locale_Translate::_($status);
    // Return status.
    return $status;
  }


  // Sets the $status of the note identified by $identifier.
  // $status is a string.
  // $import: Just write status, skip any logic.
  public function setStatus ($identifier, $status, $import = false)
  {
    // Store the authoritative copy in the filesystem.
    $file = $this->statusFile ($identifier);
    @file_put_contents ($file, $status);

    if (!$import) $this->noteEditedActions ($identifier);

    // Store a copy in the database also.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $status = Database_SQLiteInjection::no ($status);
    $query = "UPDATE notes SET status = '$status' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Gets an array of array with the possible statuses of consultation notes,
  // both raw and localized versions.
  public function getPossibleStatuses ()
  {
    // Get an array with the statuses used in the database, ordered by occurrence, most often used ones first.
    $db = self::connect ();
    $statuses = array ();
    $query = "SELECT status, COUNT(status) AS occurrences FROM notes GROUP BY status ORDER BY occurrences DESC;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $statuses[] = $row[0];
    }
    unset ($db);
    // Ensure the standard statuses are there too.
    foreach ($this->standard_statuses as $standard_status) {
      if (!in_array ($standard_status, $statuses)) {
        $statuses[] = $standard_status;
      }
    }
    // Localize the results.
    foreach ($statuses as $status) {
      $localization = $status;
      if (in_array ($status, $this->standard_statuses)) $localization = Locale_Translate::_($status);
      $localized_status = array ($status, $localization);
      $localized_statuses [] = $localized_status;
    }
    // Return result.
    return $localized_statuses;
  }


  // Returns the severity of a note as a number.
  public function getRawSeverity ($identifier)
  {
    $file = $this->severityFile ($identifier);
    @$severity = file_get_contents ($file);
    if ($severity === false) return 2;
    return (integer) $severity;
  }


  // Returns the severity of a note as a localized string.
  public function getSeverity ($identifier)
  {
    $severity = $this->getRawSeverity ($identifier);
    $severity = $this->standard_severities[$severity];
    if ($severity == "") $severity = "Normal";
    $severity = Locale_Translate::_($severity);
    return $severity;
  }


  // Sets the $severity of the note identified by $identifier.
  // $severity is a number.
  public function setRawSeverity ($identifier, $severity)
  {
    // Update the file system.
    $file = $this->severityFile ($identifier);
    file_put_contents ($file, $severity);
    
    $this->noteEditedActions ($identifier);
    
    // Update the database also.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $severity = Database_SQLiteInjection::no ($severity);
    $query = "UPDATE notes SET severity = $severity WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Gets an array with the possible severities.
  public function getPossibleSeverities ()
  {
    for ($i = 0; $i <= 5; $i++) {
      $severities[] = array ($i, Locale_Translate::_($this->standard_severities[$i]));
    }
    return $severities;
  }


  public function getModified ($identifier)
  {
    $file = $this->modifiedFile ($identifier);
    @$modified = file_get_contents ($file);
    if ($modified) return $modified;
    return 0;
  }


  public function setModified ($identifier, $time)
  {
    // Update the filesystem.
    $file = $this->modifiedFile ($identifier);
    @file_put_contents ($file, $time);
    // Update the database.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $time = Database_SQLiteInjection::no ($time);
    $query = "UPDATE notes SET modified = $time WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
    // Update checksum.
    $this->updateChecksum ($identifier);
  }


  // Takes actions when a note has been edited.
  private function noteEditedActions ($identifier)
  {
    // Update 'modified' field and calculate checksum.
    $modified = time();
    $this->setModified ($identifier, $modified);
  }


  // Returns an array of duplicate note identifiers selected.
  public function selectDuplicateNotes ($rawpassage, $summary, $contents)
  {
    $db = self::connect ();
    $identifiers = array ();
    $rawpassage = Database_SQLiteInjection::no ($rawpassage);
    $summary = Database_SQLiteInjection::no ($summary);
    $contents = Database_SQLiteInjection::no ($contents);
    $query = "SELECT identifier FROM notes WHERE passage = '$rawpassage' AND summary = '$summary' AND contents = '$contents';";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $identifiers []= $row[0];
    }
    unset ($db);
    return $identifiers;
  }


  public function updateSearchFields ($identifier)
  {
    // The search field is a combination of the summary and content converted to clean text.
    // It enables us to search with wildcards before and after the search query.
    $noteSummary = $this->getSummary ($identifier);
    $noteContents = $this->getContents ($identifier);
    $cleanText = $noteSummary . "\n" . Filter_Html::html2text ($noteContents);
    // Bail out if the search field is already up to date.
    if ($cleanText == $this->getSearchField ($identifier)) return;
    // Update the field.
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $cleanText = Database_SQLiteInjection::no ($cleanText);
    $query = "UPDATE notes SET cleantext = '$cleanText' WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  public function getSearchField ($identifier)
  {
    $db = self::connect ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $query = "SELECT cleantext FROM notes WHERE identifier = $identifier;";
    $result = Database_SQLite::query ($db, $query);
    $value = "";
    foreach ($result as $row) {
      $value = $row[0];
    }
    unset ($db);
    return $value;
  }


  /**
  * Searches the notes.
  * Returns an array of note identifiers.
  * $search: Contains the text to search for.
  * $bibles: Array of Bibles the notes should refer to.
  */
  public function searchNotes ($search, $bibles)
  {
    $identifiers = array ();

    $search = trim ($search);
    if ($search == "") return $identifiers;

    // SQL SELECT statement.
    $query = Filter_Sql::notesSelectIdentifier ();

    // SQL fulltext search statement sorted on relevance.
    $query .= Filter_Sql::notesOptionalFulltextSearchRelevanceStatement ($search);

    // SQL FROM ... WHERE statement.
    $query .= Filter_Sql::notesFromWhereStatement ();

    // Consider text contained in notes.
    $query .= Filter_Sql::notesOptionalFulltextSearchStatement ($search);

    // Consider Bible constraints:
    // * A user has access to notes that refer to Bibles the user has access to.
    // * A note can be a general one, not referring to any specific Bible.
    //   Select such notes also.
    $bibles [] = "";
    $query .= " AND (bible = '' ";
    foreach ($bibles as $bible) {
      $bible = Database_SQLiteInjection::no ($bible);
      $query .= " OR bible = '$bible' ";
    }
    $query .= " ) ";

    // Notes get ordered on relevance of search hits.
    $query .= Filter_Sql::notesOrderByRelevanceStatement ();

    // Complete query.
    $query .= ";";

    $db = self::connect ();
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $identifiers []= $row[0];
    }
    unset ($db);

    return $identifiers;
  }


  public function markForDeletion ($identifier)
  {
    $file = $this->expiryFile ($identifier);
    // Delete after 7 days.
    file_put_contents ($file, 7);
  }


  public function unmarkForDeletion ($identifier)
  {
    $file = $this->expiryFile ($identifier);
    @unlink ($file);
  }


  public function isMarkedForDeletion ($identifier)
  {
    $file = $this->expiryFile ($identifier);
    return file_exists ($file);
  }


  public function touchMarkedForDeletion ()
  {
    $identifiers = $this->getIdentifiers ();
    foreach ($identifiers as $identifier) {
      if ($this->isMarkedForDeletion ($identifier)) {
        $file = $this->expiryFile ($identifier);
        $days = file_get_contents ($file);
        $days = (integer) $days;
        $days--;
        file_put_contents ($file, $days);
      }
    }
  }


  public function getDueForDeletion ()
  {
    $deletes = array ();
    $identifiers = $this->getIdentifiers ();
    foreach ($identifiers as $identifier) {
      if ($this->isMarkedForDeletion ($identifier)) {
        $file = $this->expiryFile ($identifier);
        $days = file_get_contents ($file);
        $days = (integer) $days;
        if ($days <= 0) {
          $deletes [] = $identifier;
        }
      }
    }
    return $deletes;
  }


  public function debug ()
  {
    $db = self::connect ();
    $query = "SELECT * FROM notes;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i <= 11; $i++) unset ($row[$i]);
      var_dump ($row);
    }
    unset ($db);
  }


  // Writes the $checksum for note $identifier in the database.
  public function setChecksum ($identifier, $checksum)
  {
    // Do not write the checksum if it is already up to date.
    if ($checksum == $this->getChecksum ($identifier)) return;
    // Write the checksum to the database.
    $this->deleteChecksum ($identifier);
    $db = self::connect_checksums ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $checksum = Database_SQLiteInjection::no ($checksum);
    $query = "INSERT INTO checksums  VALUES ($identifier, '$checksum');";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // Reads the checksum for note $identifier from the database.
  public function getChecksum ($identifier)
  {
    $db = self::connect_checksums ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $query = "SELECT checksum FROM checksums WHERE identifier = $identifier;";
    $result = Database_SQLite::query ($db, $query);
    $value = "";
    foreach ($result as $row) {
      $value = $row[0];
    }
    unset ($db);
    return $value;
  }

  
  // Deletes the $checksum for note $identifier from the database.
  public function deleteChecksum ($identifier)
  {
    $db = self::connect_checksums ();
    $identifier = Database_SQLiteInjection::no ($identifier);
    $query = "DELETE FROM checksums WHERE identifier = $identifier;";
    Database_SQLite::exec ($db, $query);
    unset ($db);
  }


  // The function calculates the checksum of the note signature,
  // and writes it to the filesystem.
  public function updateChecksum ($identifier)
  {
    $checksum = array ();
    $checksum [] = $this->getModified ($identifier);
    $checksum [] = $this->getAssignees ($identifier);
    $checksum [] = $this->getSubscribers ($identifier);
    $checksum [] = $this->getBible ($identifier);
    $checksum [] = $this->getPassages ($identifier);
    $checksum [] = $this->getRawStatus ($identifier);
    $checksum [] = $this->getRawSeverity ($identifier);
    $checksum [] = $this->getSummary ($identifier);
    $checksum [] = $this->getContents ($identifier);
    $checksum = serialize ($checksum);
    $checksum = md5 ($checksum);
    $this->setChecksum ($identifier, $checksum);
  }
  

  // Reads the checksum for the notes given in array $identifiers from the database.
  public function getMultipleChecksum ($identifiers)
  {
    $db = self::connect_checksums ();
    $checksums = array ();
    foreach ($identifiers as $identifier) {
      $identifier = Database_SQLiteInjection::no ($identifier);
      $query = "SELECT checksum FROM checksums WHERE identifier = $identifier;";
      $result = Database_SQLite::query ($db, $query);
      $value = "";
      foreach ($result as $row) {
        $value = $row[0];
      }
      $checksums [] = $value;
    }
    unset ($db);
    $checksum = implode ("", $checksums);
    $checksum = md5 ($checksum);
    return $checksum;
  }

  
  // Internal function.
  private function getBibleSelector ($bibles)
  {
    if (is_array ($bibles)) {
      $bibles = array_values ($bibles);
      $bibles [] = ""; // Select general note also
      $bibleSelector = " AND (";
      foreach ($bibles as $offset => $bible) {
        $bible = Database_SQLiteInjection::no ($bible);
        if ($offset) $bibleSelector .= " OR ";
        $bibleSelector .= " bible = '$bible' ";
      }
      $bibleSelector .= ")";
      return $bibleSelector;
    }
    return "";
  }


  // This function gets the identifiers for notes
  // within the note identifier range of $lowId to $highId
  // which refer to any Bible in the array of $bibles
  // or refer to no Bible.
  public function getNotesInRangeForBibles ($lowId, $highId, $bibles)
  {
    $db = self::connect ();
    $identifiers = array ();
    $lowId = Database_SQLiteInjection::no ($lowId);
    $highId = Database_SQLiteInjection::no ($highId);
    $bibleSelector = $this->getBibleSelector ($bibles);
    $query = "SELECT identifier FROM notes WHERE identifier >= $lowId AND identifier <= $highId $bibleSelector ORDER BY identifier;";
    $result = Database_SQLite::query ($db, $query);
    foreach ($result as $row) {
      $identifiers [] = $row[0];
    }
    unset ($db);
    return $identifiers;
  }
  
  
  private function availability_flag ()
  {
    $path = dirname (__DIR__) . "/databases/notes.busy";
    return $path;
  }


  // Sets whether the notes databases are $available, as a boolean.
  public function set_availability ($available)
  {
    if ($available) {
      @unlink ($this->availability_flag ());
    } else {
      file_put_contents ($this->availability_flag (), "");
    }
  }


  // Returns whether the notes databases are available, as a boolean.
  public function available ()
  {
    if (file_exists ($this->availability_flag ())) return false;
    return true;
  }


}


?>
