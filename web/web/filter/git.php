<?php


class Filter_Git
{

  /**
  * This filter takes the Bible data as it is stored in a git repository, 
  * using the layout in books and chapters such as is used in Bibledit-Gtk,
  * and transfers this information into Bibledit-Web's Bible database.
  * The $directory is where the file data resides.
  * It overwrites whatever data was in the $bible in the database.
  * $progress: whether to show progress.
  */
  public function bibleFiledata2database ($directory, $bible, $progress = false)
  {
    // Maintain a list of chapters that were stored.
    $stored_chapters = array ();
    // Go through the Bible book names in $directory.
    $book_names = scandir($directory);
    $book_names = array_diff ($book_names, array ('.', '..'));
    $book_names = array_values ($book_names);
    $database_books = Database_Books::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_snapshots = Database_Snapshots::getInstance ();
    foreach ($book_names as $book_name) {
      $book_id = $database_books->getIdFromEnglish ($book_name);
      if ($book_id > 0) {
        if ($progress) echo "$book_name\n";
        $book_directory = "$directory/$book_name";
        // Go through the chapters in the book.
        $chapter_numbers = scandir($book_directory);
        $chapter_numbers = array_diff ($chapter_numbers, array ('.', '..'));
        $chapter_numbers = array_values ($chapter_numbers);
        foreach ($chapter_numbers as $chapter_number) {
          // Go ahead if $chapter_number is numerical.
          if (is_numeric ($chapter_number)) {
            // Store data into database, but only when the new data differs from existing data.
            // This reduces the number of snapshots created, and the time taken to complete the operation,
            // assuming that in most cases the old does not differ from the new, which is a reasonable assumption.
            $data = file_get_contents ("$directory/$book_name/$chapter_number/data");
            $old_data = $database_bibles->getChapter ($bible, $book_id, $chapter_number);
            if ($data != $old_data) {
              $database_bibles->storeChapter ($bible, $book_id, $chapter_number, $data);
              $database_snapshots->snapChapter ($bible, $book_id, $chapter_number, $data, false);
            }
            // List the chapter number.
            $stored_chapters [] = array ($book_id, $chapter_number);
          }
        }
      }
    }
    // Delete any chapters not stored above.
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $item = array ($book, $chapter);
        if (!in_array ($item, $stored_chapters)) {
          $database_bibles->deleteChapter ($bible, $book, $chapter);
        }
      }
    }
  }


  /**
  * This filter takes the Bible data as it is stored in Bibledit-Web's database, 
  * and transfers this information into the layout in books and chapters
  * such as is used in Bibledit-Gtk into $directory.
  * The $directory is supposed to be completely empty, 
  * apart from a .git directory which may be there.
  */
  public function bibleDatabase2filedata ($bible, $directory, $progress = false)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $book_name = $database_books->getEnglishFromId ($book);
      if ($progress) echo "$book_name\n";
      mkdir ("$directory/$book_name");
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        mkdir ("$directory/$book_name/$chapter");
        $data = $database_bibles->getChapter ($bible, $book, $chapter);
        file_put_contents ("$directory/$book_name/$chapter/data", $data);
      }
    }
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
    $notescounter = 0; // For progress counter.
    $database_notes = Database_Notes::getInstance ();
    // Select all notes identifiers. Proper values should be passed to the selection routine, so it gives all notes.
    // E.g. a sufficiently high $userlevel is given, so all notes are included.
    $identifiers = $database_notes->getIdentifiers ();
    foreach ($identifiers as $identifier) {
      if (($notescounter % 1000) == 0) if ($progress) echo "$notescounter\n";
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
      file_put_contents ($filename, implode ("\n", $data));
    }
    if ($progress) echo "$notescounter\n";
  }

  /**
  * The function transfers the file data, as it is in the git repository, 
  * into the notes database.
  * While transferring this information, care is taken to minimize the writes into the database.
  * Each field, as it is in the file data, is compared with the corresponding field in the database.
  * When the fields are the same, no writes occur.
  */
  public function notesFiledata2database ($directory, $progress = false)
  {
    $database_notes = Database_Notes::getInstance ();
    $notes_logic = Notes_Logic::getInstance();
    $stored_identifiers = array (); // Maintain a list of note identifiers stored.
    $notescounter = 0; // For progress counter.
    foreach (new DirectoryIterator ($directory) as $fileInfo) {
      if($fileInfo->isDot()) continue;
      if($fileInfo->isDir()) continue; // Exclude directories, e.g. the ".git" one.
      if (($notescounter % 1000) == 0) if ($progress) echo "$notescounter\n";
      $notescounter++;
      $identifier = $fileInfo->getFilename();
      $stored_identifiers [] = $identifier;
      $note_updated = false;
      if (!$database_notes->identifierExists ($identifier)) {
        // Somebody may have created a new note in the git repository. 
        // In this case, that note should also be created in our notes database.
        // A dummy note is created here, and to be updated as we go along.
        $id = $database_notes->storeNewNote ("bible", 1, 2, 3, "summary", "contents", false);
        $note_updated = true;
        // The dummy note that was created has its own identifier.
        // This identifier is to be updated to the one in the git repository.
        $database_notes->setIdentifier ($id, $identifier);
      }
      $filename = "$directory/$identifier";
      $filedata = explode ("\n", file_get_contents ($filename));
      unset ($modified);
      unset ($fielddata);
      // While going through the file's data, the order of the heading is of critical importance.
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
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Bible:":
            // This heading follows the Subscribers: heading, hence the note's subscribers are stored here.
            if (!isset ($fielddata)) $fielddata = array ();
            $subscribers = $database_notes->getSubscribers ($identifier);
            if ($fielddata != $subscribers) {
              $database_notes->setSubscribers ($identifier, $fielddata);
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Passages:":
            // This heading follows the Bible: heading, hence the note's Bible is stored here.
            if (!isset ($fielddata)) $fielddata = array ();
            @$bible = $fielddata[0];
            if ($bible != $database_notes->getBible ($identifier)) {
              $database_notes->setBible ($identifier, $bible);
              $note_updated = true;
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
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Severity:":
            // This heading follows the Status: heading, hence the note's status is stored here.
            @$status = $fielddata[0];
            if ($status != $database_notes->getStatus ($identifier)) {
              $database_notes->setStatus ($identifier, $status);
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Privacy:":
            // This heading follows the Severity: heading, hence the note's severity is stored here.
            @$severity = $fielddata[0];
            if ($severity != $database_notes->getRawSeverity ($identifier)) {
              $database_notes->setRawSeverity ($identifier, $severity);
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Summary:":
            // This heading follows the Privacy: heading, hence the note's privacy is stored here.
            @$privacy = $fielddata[0];
            if ($privacy != $database_notes->getPrivacy ($identifier)) {
              $database_notes->setPrivacy ($identifier, $privacy);
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          case "Contents:":
            // This heading follows the Summary: heading, hence the note's summary is stored here.
            @$summary = $fielddata[0];
            if ($summary != $database_notes->getSummary ($identifier)) {
              $database_notes->setSummary ($identifier, $summary);
              $note_updated = true;
            }
            unset ($fielddata);
            break;
          default:
            $fielddata [] = $fileline;
            break;
        }
      }
      // Contents is last in the file. check whether to store it.
      $contents = implode ("\n", $fielddata);
      if ($contents != $database_notes->getContents ($identifier)) {
        $note_updated = true;
      }
      // If needed, invoke the notifications system.
      if ($note_updated) {
        $notes_logic->handlerUpdateNote ($identifier);
      }
      // Writing the note's contents is done after the notification system.
      // This is necessary to avoid possible race conditions.
      // A race condition could occur two installations of Bibledit-Web share their notes through a git repository,
      // and each of these installations would apply its own notifications updates.
      // Each of them would then add content each time, one after the other.
      // Writing content after the notificatons prevents this.
      if ($contents != $database_notes->getContents ($identifier)) {
        $database_notes->setContents ($identifier, $contents);
        $note_updated = true;
      }
      // At the end of all, since the note' modification time may have changed as a result
      // of updating the note's fields, the modification time should be (re)set here to the 
      // value that was read from the git repository's note file.
      if (isset ($modified)) {
        if ($modified != $database_notes->getModified ($identifier)) {
          $database_notes->setModified ($identifier, $modified);
        }
      }
    }
    if ($progress) echo "$notescounter\n";

    // Delete any notes which were not in the git repository.
    $identifiers = $database_notes->getIdentifiers ();
    $differences = array_diff ($identifiers, $stored_identifiers);
    foreach ($differences as $identifier) {
      $notes_logic->handlerDeleteNote ($identifier);
      $database_notes->delete ($identifier);
    }
  }


  /**
  * This filter takes a git repository as it is stored in .git
  * in $directory, and transfers this information into Bibledit-Web's
  * Repository database under name $bible.
  * It also works with consultation notes.
  * $progress - whether to show progress.
  */
  public function repository2database ($directory, $bible, $progress = false)
  {
    // Put the .git directory in an uncompressed tar ball (-czf would compress it).
    $filename = "git.tar";
    $command = "cd $directory; tar";
    if ($progress) $command = $command . " -v ";
    $command = $command . " -cf $filename .git";
    passthru ($command);
    $data = fread(fopen("$directory/$filename", "r"), filesize("$directory/$filename"));
    $database_repositories = Database_Repositories::getInstance();
    $database_repositories->storeRepository ($bible, $data);    
  }
  

  /**
  * This filter takes the git repository for $bible
  * as it is stored in Bibledit-Web's database, 
  * and puts it in a .git directory in $directory.
  * There should not be an existing .git directory in $directory,
  * but other relevant data may be there.
  * It also works with consultation notes.
  */
  public function database2repository ($bible, $directory)
  {
    $database_repositories = Database_Repositories::getInstance();
    $data = $database_repositories->getRepository ($bible);
    $filename = "git.tar";
    file_put_contents ("$directory/$filename", $data);
    system ("cd $directory; tar -xf $filename");
    unlink ("$directory/$filename");
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
      
      

}


?>
