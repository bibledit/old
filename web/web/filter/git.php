<?php


class Filter_Git
{

  /**
  * This filter moves one chapter of the Bible data as it is stored in a git repository, 
  * using the layout in books and chapters such as is used in Bibledit-Gtk,
  * and transfers this chapter into Bibledit-Web's Bible database.
  * The $directory is where the file data resides.
  * The $bible is the Bible.
  * The $output is an output line of by git pull.
  */
  public function bibleFiledata2database ($directory, $bible, $output)
  {
    // The $output contains one line of the output of "git pull".
    // A normal action is when a chapter of the Bible is updated as a result of "git pull". 
    // Example:
    // Updating 4807e84..0afa0e3
    // Fast forward
    // 3 John/1/data |    2 +-
    // 1 files changed, 1 insertions(+), 1 deletions(-)

    // Find out if the $output refers to a chapter.
    if (strlen($output) <= 5) return;
    $bits = explode (DIRECTORY_SEPARATOR, $output);
    if (count ($bits) < 3) return;
    $database_books = Database_Books::getInstance ();
    $bookname = trim ($bits[0]);
    $bookid = $database_books->getIdFromEnglish ($bookname);
    if ($bookid == 0) return;
    $rawchapter = trim ($bits[1]);
    if ($rawchapter == "") return;
    $chapter = Filter_Numeric::integer_in_string ($rawchapter);
    if ($chapter != $rawchapter) return;
    $datafile = trim ($bits[2]);
    $datafile = substr ($datafile, 0, 4);
    if ($datafile != "data") return;
    $datafile = "$directory/$bookname/$chapter/$datafile";

    // Databases.
    $database_bibles = Database_Bibles::getInstance ();
    $database_snapshots = Database_Snapshots::getInstance ();
    $database_logs = Database_Logs::getInstance();

    // If the $datafile does not exist, it means that the chapter was deleted.
    if (!file_exists ($datafile)) {
      $database_bibles->deleteChapter ($bible, $bookid, $chapter);
      $database_logs->log (gettext ("The collaboration system deleted a chapter") . ": $bible $bookname $chapter");
      return;
    }
    
    // At this point we are sure that the chapter exists, and has been changed. Read the file's contents.
    $contents = file_get_contents ($datafile);

    // Check on a conflict, and resolve that automatically.
    // Todo Filter_Git::resolveConflict ($contents, $datafile);

    // Store data into database.
    $database_bibles->storeChapter ($bible, $bookid, $chapter, $contents);
    $database_snapshots->snapChapter ($bible, $bookid, $chapter, $contents, false);
    $database_logs->log (gettext ("The collaboration system updated a chapter") . ": $bible $bookname $chapter");
  }


  /**
  * This filter takes the Bible data as it is stored in Bibledit-Web's database, 
  * and transfers this information into the layout in books and chapters
  * such as is used in Bibledit-Gtk into $directory.
  * The $directory may contain other data. This data will not normally be affected.
  */
  public function bibleDatabase2filedata ($bible, $directory, $progress = false)
  {
    $success = true;
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $book_name = $database_books->getEnglishFromId ($book);
      if ($progress) echo "$book_name ";
      $bookdir = "$directory/$book_name";
      if (!is_dir ($bookdir)) mkdir ($bookdir);
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $chapterdir = "$bookdir/$chapter";
        if (!is_dir ($chapterdir)) mkdir ($chapterdir);
        $data = $database_bibles->getChapter ($bible, $book, $chapter);
        if (file_put_contents ("$chapterdir/data", $data) === false)
          $success = false;
      }
    }
    if ($progress) echo "\n";
    return $success;
  }


  /**
  * This filter takes the Consultations Notes as these are stored in Bibledit-Web's database, 
  * and transfers this information to the file system,
  * each notes having a file.
  * The $directory is supposed to be completely empty, 
  * apart from a .git directory which may be there.
  */
  public function notesDatabase2filedata ($directory, $progress = false)
  {
    $success = true;
    $notescounter = 0; // For progress counter.
    $database_notes = Database_Notes::getInstance ();
    // Select all notes identifiers. Proper values should be passed to the selection routine, so it gives all notes.
    // E.g. a sufficiently high $userlevel is given, so all notes are included.
    $identifiers = $database_notes->getIdentifiers ();
    foreach ($identifiers as $identifier) {
      if (($notescounter % 1000) == 0) if ($progress) echo "$notescounter ";
      $notescounter++;
      // The notes $identifier becomes the filename.
      $filename = "$directory/$identifier";
      // Assemble the file's data.
      $data = array ();
      $data [] = "Modified:";
      $data [] = $database_notes->getModified ($identifier);
      $data [] = "Assignees:";
      $assignees = $database_notes->getAssignees ($identifier);
      foreach ($assignees as $assignee) {
        $data [] = $assignee;
      }
      $data [] = "Subscribers:";
      $subscribers = $database_notes->getSubscribers ($identifier);
      foreach ($subscribers as $subscriber) {
        $data [] = $subscriber;
      }
      $data [] = "Bible:";
      $data [] = $database_notes->getBible ($identifier);
      $data [] = "Passages:";
      $passages = $database_notes->getPassages ($identifier);
      foreach ($passages as $passage) {
        $data [] = trim ($database_notes->encodePassage ($passage[0], $passage[1], $passage[2]));
      }
      $data [] = "Status:";
      $data [] = $database_notes->getStatus ($identifier);
      $data [] = "Severity:";
      $data [] = $database_notes->getRawSeverity ($identifier);
      $data [] = "Privacy:";
      $data [] = $database_notes->getPrivacy ($identifier);
      $data [] = "Summary:";
      $data [] = $database_notes->getSummary ($identifier);
      $data [] = "Contents:";
      $data [] = $database_notes->getContents ($identifier);
      // Save the data to file.
      if (file_put_contents ($filename, implode ("\n", $data)) === false)
        $success = false;
    }
    if ($progress) echo "$notescounter\n";
    return $success;
  }


  /**
  * The function transfers the file data, as it is in the git repository, 
  * into the notes database.
  * It transfers one note at a time, and then returns.
  * $directory: the git directory.
  * $output: one line of output of a 'git pull' command.
  */
  public function notesFiledata2database ($directory, $output)
  {
    // The $output contains one line of the output of "git pull".
    // A normal action is when a note is updated as a result of "git pull". 
    // Example:
    // Updating e0997ff..4ac44e9
    // Fast-forward
    //  768238653 |    4 ++--
    //  1 files changed, 2 insertions(+), 2 deletions(-)

    // Find out if the $output refers to a note.
    if (strlen($output) <= 5) return;
    $bits = explode ("|", $output);
    if (count ($bits) != 2) return;
    $identifier = trim ($bits[0]);
    if ($identifier == 0) return;
    if ($identifier == "") return;
    if ($identifier != Filter_Numeric::integer_in_string ($identifier)) return;
    $datafile = "$directory/$identifier";

    // Databases and logic.
    $database_notes = Database_Notes::getInstance ();
    $notes_logic = Notes_Logic::getInstance();
    $database_logs = Database_Logs::getInstance();

    // If the $datafile does not exist, it means that the note was deleted by the collaboration system.
    if (!file_exists ($datafile)) {
      $notes_logic->handlerDeleteNote ($identifier);
      $database_notes->delete ($identifier);
      $database_logs->log (gettext ("The collaboration system deleted a note") . ": $identifier");
      return;
    }
    
    // At this point we are sure that the note exists, and has been changed, or is new.

    // Read the file's contents.
    $contents = file_get_contents ($datafile);

    // Check on a conflict, and resolve that automatically.
    // Todo Filter_Git::resolveConflict ($contents, $datafile);

    // Start importing the note.

    if (!$database_notes->identifierExists ($identifier)) {
      // Somebody created a new note in the git repository.
      // That note should also be created in our notes database.
      // A dummy note is created here, to be updated as we go along.
      $id = $database_notes->storeNewNote ("bible", 1, 2, 3, "summary", "contents", false);
      // The dummy note that was created has its own identifier.
      // This identifier is to be updated to the one in the git repository.
      $database_notes->setIdentifier ($id, $identifier);
    }

    $filedata = explode ("\n", $contents);

    unset ($modified);
    unset ($fielddata);
    // While going through the file's data, the order of the headings is of critical importance.
    foreach ($filedata as $fileline) {
      switch ($fileline) {
        case "Modified:":
          // This heading is at the top of the file. There's nothing to be done here yet.
          unset ($fielddata);
          break;
        case "Assignees:":
          // This heading follows the Modified: heading, hence the note's $modified value gets cached here.
          if (is_array ($fielddata)) $modified = $fielddata[0];
          unset ($fielddata);
          break;
        case "Subscribers:":
          // This heading follows the Assignees: heading, hence the note's assignees are stored here.
          if (!isset ($fielddata)) $fielddata = array ();
          $assignees = $database_notes->getAssignees ($identifier);
          if ($fielddata != $assignees) {
            $database_notes->setAssignees ($identifier, $fielddata);
          }
          unset ($fielddata);
          break;
        case "Bible:":
          // This heading follows the Subscribers: heading, hence the note's subscribers are stored here.
          if (!isset ($fielddata)) $fielddata = array ();
          $subscribers = $database_notes->getSubscribers ($identifier);
          if ($fielddata != $subscribers) {
            $database_notes->setSubscribers ($identifier, $fielddata);
          }
          unset ($fielddata);
          break;
        case "Passages:":
          // This heading follows the Bible: heading, hence the note's Bible is stored here.
          if (!isset ($fielddata)) $fielddata = array ();
          @$bible = $fielddata[0];
          if ($bible != $database_notes->getBible ($identifier)) {
            $database_notes->setBible ($identifier, $bible);
          }
          unset ($fielddata);
          break;
        case "Status:":
          // This heading follows the Passages: heading, hence the note's passages are stored here.
          if (!isset ($fielddata)) $fielddata = array ();
          $passages = $database_notes->getPassages ($identifier);
          foreach ($passages as &$passage) {
            $passage = $database_notes->encodePassage ($passage[0], $passage[1], $passage[2]);
            $passage = trim ($passage);
          }
          if ($passages != $fielddata) {
            $passages = array ();
            foreach ($fielddata as $data) {
              $passages [] = $database_notes->decodePassage ($data);
            }
            $database_notes->setPassages ($identifier, $passages);
          }
          unset ($fielddata);
          break;
        case "Severity:":
          // This heading follows the Status: heading, hence the note's status is stored here.
          @$status = $fielddata[0];
          if ($status != $database_notes->getStatus ($identifier)) {
            $database_notes->setStatus ($identifier, $status);
          }
          unset ($fielddata);
          break;
        case "Privacy:":
          // This heading follows the Severity: heading, hence the note's severity is stored here.
          @$severity = $fielddata[0];
          if ($severity != $database_notes->getRawSeverity ($identifier)) {
            $database_notes->setRawSeverity ($identifier, $severity);
          }
          unset ($fielddata);
          break;
        case "Summary:":
          // This heading follows the Privacy: heading, hence the note's privacy is stored here.
          @$privacy = $fielddata[0];
          if ($privacy != $database_notes->getPrivacy ($identifier)) {
            $database_notes->setPrivacy ($identifier, $privacy);
          }
          unset ($fielddata);
          break;
        case "Contents:":
          // This heading follows the Summary: heading, hence the note's summary is stored here.
          @$summary = $fielddata[0];
          if ($summary != $database_notes->getSummary ($identifier)) {
            $database_notes->setSummary ($identifier, $summary);
          }
          unset ($fielddata);
          break;
        default:
          $fielddata [] = $fileline;
          break;
      }
    }
    // Contents is last in the file to store.
    $contents = implode ("\n", $fielddata);
    // Invoke the notifications system.
    $notes_logic->handlerUpdateNote ($identifier);
    // Writing the note's contents is done after the notification system.
    // This is necessary to avoid possible race conditions.
    // A race condition could occur two installations of Bibledit-Web share their notes through a git repository,
    // and each of these installations would apply its own notifications updates.
    // Each of them would then add content each time, one after the other.
    // Writing content after the notificatons prevents this.
    if ($contents != $database_notes->getContents ($identifier)) {
      $database_notes->setContents ($identifier, $contents);
    }
    // At the end of all, since the note' modification time may have changed as a result
    // of updating the note's fields, the modification time should be (re)set here to the 
    // value that was read from the git repository's note file.
    if (isset ($modified)) {
      if ($modified != $database_notes->getModified ($identifier)) {
        $database_notes->setModified ($identifier, $modified);
      }
    }

    // Log entry.
    $database_logs->log (gettext ("The collaboration system created or updated a note") . ": $identifier");
  }


  /**
  * Bibledit-Web accesses secure git repositories through ssh.
  * This function sets up both git and ssh to use a configuration file
  * and a private key.
  * The $url is the URL of the secure git repository.
  */
  public function git_config ($url)
  {
    // Temporal directory where to put the config files.
    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);

    // Extract the host name from the $url.
    $host = parse_url ($url, PHP_URL_HOST);

$config_data = <<<EOD
Host $host
     IdentityFile $directory/key
     BatchMode yes
     StrictHostKeyChecking no
EOD;
    file_put_contents ("$directory/config", $config_data);
    
    $database_config_general = Database_Config_General::getInstance();
    file_put_contents ("$directory/key", $database_config_general->getPrivateSshKey ());
    chmod ("$directory/key", 0600);

$git_ssh_data = <<<EOD
#!/bin/sh
exec ssh -F $directory/config $*
EOD;
    file_put_contents ("$directory/git_ssh", $git_ssh_data);
    chmod ("$directory/git_ssh", 0777);
    putenv ("GIT_SSH=$directory/git_ssh");

    return $directory;    
  }


  /**
  * Bibledit-Web accesses secure git repositories through ssh.
  * This function remove the private key set up by git_config
  */
  public function git_un_config ($directory)
  {
    unlink ("$directory/key");
  }


  /**
  * This function returns the directory of the git repository belonging to $object.
  */
  public function git_directory ($object)
  {
    $directory = dirname (dirname (__FILE__));
    $directory = "$directory/git/$object";
    return $directory;
  }
  
  
  public function filedata2database ()
  {
    $database_git = Database_Git::getInstance();
    while ($database_git->get()) {
      $entry = $database_git->get ();
      $directory = $entry['directory'];
      $output = $entry['output'];
      $path = dirname (dirname (__FILE__));
      $path = "$path/git/";
      $object = substr ($directory, strlen ($path));
      if ($object == "consultationnotes") {
        Filter_Git::notesFiledata2database ($directory, $output);
      } else {
        Filter_Git::bibleFiledata2database ($directory, $object, $output);
      }
      $database_git->delete ($directory, $output);
    }
  }
  
  
  /*
   * This resolves a git conflict in $contents.
   * If there is a conflict, it modifies $contents.
   * It saves the modified $contents to $filename.  
   */
  public function resolveConflict (&$contents, $filename) // Todo
  {
    /*
    The first thing to do is to see whether there is a conflict.
    In most cases all is fine. Then the function returns.
    If there is a conflicting merge after doing a 'git pull', the $contents looks like this:
    
    <<<<<<< HEAD:3 John/1/data
    \v 1 xFrom the church leader.
    =======
    \v 1 xxFrom the church leader.
    >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data

    or like this:

    \c 1
    <<<<<<< HEAD:3 John/1/data
    \v 1 my text.
    =======
    \v 1 server's text.
    >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data
    \v 2

    */
    if (strpos ($contents, "<<<<<<< HEAD") === FALSE) return;

    $database_logs = Database_Logs::getInstance ();
    
    // The $contents is a string, change it to an array.
    $lines = explode ("\n", $contents);

    // Set about to resolve the conflict.
    $newdata = array ();
    $withinmine = false;
    $withinserver = false;
    foreach ($lines as $line) {
      // Find out when we've got a marker, no actual data.    
      $minemarker = (strpos ($line, "<<<<<<< HEAD") !== FALSE);
      $separatormarker = ($line == "=======");
      $servermarker = (strpos ($line, ">>>>>>> ") !== FALSE);
      // Do conflict management only if we've actual data.
      $takeit = false;
      if (!$minemarker && !$separatormarker && !$servermarker) {
        if ($withinmine) {
          $database_logs->log ("Merge conflict, my data: " . $line);
          //if ($conflicthandling == $TakeMe)
          if (false)
            $takeit = true;
        } else if ($withinserver) {
          $database_logs->log ("Merge conflict, server data: " . $line);
          //if ($conflicthandling == $TakeServer)
          if (true)
            $takeit = true;
        } else {
          $takeit = true;
        }
      }
      if ($takeit) {
        $newdata [] = $line;
      } else {
      }
      // Set whether we're within a conflict, my data, or the server's data.
      // This setting applies to the next line of data.
      if ($minemarker) {
        $withinmine = true;
        $withinserver = false;
      }
      if ($separatormarker) {
        $withinmine = false;
        $withinserver = true;
      }
      if ($servermarker) {
        $withinmine = false;
        $withinserver = false;
      }
    }
    $contents = implode ("\n", $newdata);

    file_put_contents ($filename, $contents);
  }
          
      

}


?>
