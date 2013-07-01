<?php

if (php_sapi_name () != "cli") return;

require_once ("../bootstrap/bootstrap.php");

error_reporting (E_ERROR);

$object = $argv[1];
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$directory = Filter_Git::git_directory ($object);


// Take action based on whether it deals with a Bible or with the Consultation Notes.
if ($object == "consultationnotes") {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's consultation notes,
  // overwriting anything that was there before.
  echo gettext ("Copying the data from the local cloned repository, and storing it in the Consultation Notes") . "\n";
  $notescounter = 0; // For progress counter.
  foreach (new DirectoryIterator ($directory) as $fileInfo) {
    if($fileInfo->isDot()) continue;
    if($fileInfo->isDir()) continue; // Exclude directories, e.g. the ".git" one.
    if (($notescounter % 1000) == 0) echo "$notescounter\n";
    $notescounter++;
    $identifier = $fileInfo->getFilename();
    // Simulate a line of output of command 'git pull'.
    Filter_Git::notesFiledata2database ($directory, "$identifier |    4 ++--");
  }

} else {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's Bible given in $object,
  // overwriting the whole Bible that was there before.
  echo gettext ("Copying the data from the local cloned repository, and storing it in the Bible") . "\n";
  
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
      echo "$book_name\n";
      $book_directory = "$directory/$book_name";
      $chapter_numbers = scandir($book_directory);
      $chapter_numbers = array_diff ($chapter_numbers, array ('.', '..'));
      $chapter_numbers = array_values ($chapter_numbers);
      // Go through the chapters in the book.
      foreach ($chapter_numbers as $chapter_number) {
        // Go ahead if $chapter_number is numerical.
        if (is_numeric ($chapter_number)) {
          Filter_Git::bibleFiledata2database ($directory, $object, "$book_name/$chapter_number/data |    2 +-");
        }
      }
    }
  }
  
}

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_repo", 0);

?>
